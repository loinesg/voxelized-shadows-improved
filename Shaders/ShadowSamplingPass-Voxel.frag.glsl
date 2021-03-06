#version 400

// The maximum number of leaf masks that a single PCF lookup can touch
// 17x17 PCF can touch up to 3x3=9 leaf nodes
#define PCF_MAX_LOOKUPS 9

// Camera uniform buffer
layout(std140) uniform camera_data
{
    uniform vec2 _ScreenResolution;
    uniform vec3 _CameraPosition;
    uniform mat4x4 _WorldToView;
    uniform mat4x4 _ViewProjectionMatrix;
    uniform mat4x4 _ClipToWorld;
    uniform vec4 _FrustumCorners[4];
    uniform vec2 _CameraClipPlanes; // x = near, y = far
};

// voxel_data uniform buffer
layout(std140) uniform voxel_data
{
    uniform mat4x4 _WorldToVoxel;
    uniform uint _VoxelTreeHeight;
    uniform uint _TileSubdivisions;
    
    // The total number of voxels in the PCF kernel
    uniform uint _PCFSampleCount;
    
    // The number of leaf nodes visited in a PCF kernel
    uniform uint _PCFLookups;
    
    // The bitmask and offset for PCF kernel lookups.
    // Stores (xOffset, yOffset, bitmask0, bitmask1)
    // PCF_MAX_LOOKUPS values per original leaf mask index
    uniform uvec4 _PCFOffsets[64 * PCF_MAX_LOOKUPS];
};

// Scene depth texture
uniform sampler2D _MainTexture;

// Voxelized Shadow Map data
uniform usamplerBuffer _VoxelData;

in vec3 viewDir;
in vec2 texcoord;

// Output color
// Contains (shadow, 0, 0, 0)
out vec4 fragColor;

// The result of querying the voxel tree several times
// and applying PCF
struct VoxelQuery
{
    uint treeDepthReached;
    float shadowAttenuation;
};

// The result of a single tree lookup
struct LeafNodeQuery
{
    uint treeDepthReached;
    uvec2 bits; // 64 bits, 2 words, high bits in x
};

/*
 * Get the voxel-space coordinate corresponding to a world-space position.
 * The computed coordinate can be used to look up a voxel in the tree.
 */
uvec3 getVoxelCoord(vec4 worldSpacePosition)
{
    // worldSpacePosition.w must be 1
    return uvec3((_WorldToVoxel * worldSpacePosition).xyz);
}

/*
 * Computes the child index at a given depth for the specified coord.
 * Must be consistent with the cpp builder code.
 */
uint getChildIndex(uint depth, uvec3 coord)
{
    // The last inner node before the leaf nodes is treated differently.
    if(depth == _VoxelTreeHeight - 3u)
    {
        // Nodes are in a vertical stack.
        // Recover directly from the last z coord bits.
        return coord.z & 7u;
    }
    
    // Use the least significant bit for each axis.
    uvec3 childIndex = (coord >> (_VoxelTreeHeight - 1u - depth)) & 1u;

    // Combine them
    return (childIndex.x << 2) | (childIndex.y << 1) | childIndex.z;
}

/*
 * Computes the word offset from a child mask to the pointer to the
 * child with the specified index.
 */
int getChildPointerOffset(uint childMask, uint childIndex)
{
    // The uniform / mixed flag is in the second bit for each
    // child info 2-bit value
    uint mixedFlagBits = 43690u; // binary 10 10 10 10 10 10 10 10
    
    // Use only the flags up to and including the specified child index.
    mixedFlagBits = mixedFlagBits >> (14u - childIndex * 2u);
    
    // Count the number of mixed flag bits that are set.
    return bitCount(childMask & mixedFlagBits);
}

/*
 * Computes the index of a voxel within its leaf node.
 */
uint getVoxelLeafIndex(uvec3 coord)
{
    // We only use the last 3 x and y bits
    coord = coord & 7u;
    
    // Combine to form the index
    return (coord.x << 3) | coord.y;
}

LeafNodeQuery getLeafNode(uvec3 coord)
{
    // Compute which tile the coord is in
    uvec2 tile = coord.xy >> _VoxelTreeHeight;
    uint tileIndex = (tile.x * _TileSubdivisions) + tile.y;
    
    // Get the memory address of the first node to visit
    int memAddress = int(texelFetch(_VoxelData, int(tileIndex)).r);

    // Traverse inner nodes
    for(uint depth = 0u; depth <= _VoxelTreeHeight - 3u; ++depth)
    {
        // Fetch the node's child mask
        uint childIndex = getChildIndex(depth, coord);
        uint childMask = texelFetch(_VoxelData, memAddress).r >> 16;
        uint childState = (childMask >> (childIndex * 2u)) & 3u;
        
        // If uniform shadow, exit early
        if(childState < 2u)
        {
            LeafNodeQuery q;
            q.treeDepthReached = depth;
            q.bits = uvec2(4294967295u * childState);
            return q;
        }
        
        // Mixed shadow
        // Retrieve the child node memory location
        int childPtrOffset = getChildPointerOffset(childMask, childIndex);
        int childPtr = memAddress + childPtrOffset;
        memAddress = int(texelFetch(_VoxelData, childPtr).r);
    }
    
    // We have reached a leaf node.
    LeafNodeQuery q;
    q.treeDepthReached = _VoxelTreeHeight;
    q.bits = uvec2(texelFetch(_VoxelData, memAddress).r,
                 texelFetch(_VoxelData, memAddress + 1).r);
    return q;
}

/*
 * Get the shadow attenuation for the voxel with the given coordinate.
 * Also performs PCF filtering, if enabled.
 */
VoxelQuery sampleShadowTree(uvec3 coord)
{
    // Get the location of the coord within its leaf
    uint leafIndex = getVoxelLeafIndex(coord);
    
#if !defined(SHADOW_PCF_FILTER)
    
    // Get the leaf node
    LeafNodeQuery leaf = getLeafNode(coord);
    
    // Get the shadowing state of the voxel
    uint shadowing = leafIndex > 31u
        ? (leaf.bits.y >> (leafIndex-32u)) & 1u
        : (leaf.bits.x >> leafIndex) & 1u;
    
    // Return the query result
    VoxelQuery q;
    q.treeDepthReached = leaf.treeDepthReached;
    q.shadowAttenuation = float(shadowing);
    return q;
    
#else
    
    // Keep track of how many voxels are unshadowed
    // Track high and low bits separately and combine at the end
    uvec2 unshadowed = uvec2(0u);
    
    // Calculate the sum of the tree depths for debugging overlays
    uint treeDepthSum = 0u;
    
    // Process each PCF lookup
    for(uint i = 0; i < _PCFLookups; i++)
    {
        // Get the lookup data
        uvec4 lookup = _PCFOffsets[leafIndex * PCF_MAX_LOOKUPS + i];
        uvec2 offset = lookup.xy;
        uvec2 bitmask = lookup.zw;
        
        // Get the leaf coord
        uvec3 pcfCoord = uvec3(coord.xy + offset, coord.z);
        
        // Query the shadow tree
        LeafNodeQuery leaf = getLeafNode(pcfCoord);
        unshadowed += bitCount(leaf.bits & bitmask);
        treeDepthSum += leaf.treeDepthReached;
    }
    
    // Return the query result
    VoxelQuery q;
    q.treeDepthReached = treeDepthSum / 4u;
    q.shadowAttenuation = float(unshadowed.x + unshadowed.y) / float(_PCFSampleCount);
    return q;
    
#endif
}

void main()
{
    // Retrieve screen coordinate and linear depth.
    float linearDepth = texture(_MainTexture, texcoord).r;
    
    // Exit early if the depth is the skybox depth
    if(linearDepth == 1.0)
    {
        discard;
    }
    
    // Compute the world position using the camera pos, normalized
    // view dir from vertex shader and linear depth using 1 madd
    vec4 worldPos = vec4(viewDir * linearDepth + _CameraPosition, 1.0);
    
    // Get the coordinate for the voxel tree
    uvec3 voxelCoord = getVoxelCoord(worldPos);
    
    // Sample the shadow tree
    VoxelQuery result = sampleShadowTree(voxelCoord);
    
#ifdef DEBUG_SHOW_VOXEL_TREE_DEPTH
    
    // Discard overlay samples on the left half of the screen
    if(texcoord.x < 0.5) discard;
    
    // Determine the % of the tree traversed
    float traversalDepth = float(result.treeDepthReached) / (float(_VoxelTreeHeight));
    
    // Determine the resulting colour
    vec4 rootColour = vec4(0.0, 0.0, 1.0, 0.5); // blue
    vec4 leafColour = vec4(1.0, 0.0, 0.0, 0.5); // red
    fragColor = mix(rootColour, leafColour, traversalDepth);
    
#else
    
    // Output shadow
    fragColor = vec4(result.shadowAttenuation, 0, 0, 1);
    
#endif
}
