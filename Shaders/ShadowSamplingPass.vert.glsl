#version 330

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

layout(location = 0) in vec4 _position;

out vec3 viewDir;
out vec2 texcoord;

void main()
{
    // Fullscreen quad. No need to modify position
    gl_Position = _position;
    
    // Send the frustum direction vector (world space) to the frag shader
    // This allows the frag shader to recompute the world position with
    // one madd.
    // See Real-time Atmospheric Effects in Games (Crytek 2006)
    viewDir = _FrustumCorners[gl_VertexID].xyz;
    
    // Transform the position from [-1,1] to [0,1] for texcoord
    texcoord = _position.xy / 2.0 + 0.5;
}
