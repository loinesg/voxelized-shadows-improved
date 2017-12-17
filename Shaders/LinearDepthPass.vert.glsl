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

layout(std140) uniform per_object_data
{
    uniform mat4x4 _ModelToWorldPerInstance[256];
};

layout(location = 0) in vec4 _position;

#ifdef ALPHA_TEST_ON
    // Use the main texture and texcoord for alpha testing
    layout(location = 3) in vec2 _texcoord;
    out vec2 texcoord;
#endif

out float linearDistance;

void main()
{
    mat4x4 _ModelToWorld = _ModelToWorldPerInstance[gl_InstanceID];
    vec4 worldSpacePos = (_ModelToWorld * _position);
    gl_Position = _ViewProjectionMatrix * worldSpacePos;
    
#ifdef ALPHA_TEST_ON
    // Texcoord only needed for alpha test texture lookups
    texcoord = _texcoord;
#endif
    
    // Compute the linear distance to the camera.
    // Normalize it so that 1 is on the far clip plane.
    linearDistance = (_WorldToView * worldSpacePos).z / _CameraClipPlanes.y;
}
