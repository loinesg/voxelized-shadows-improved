#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <string>
#include <vector>

using namespace std;


enum ShaderFeature
{
    // Enables texturing
    SF_Texture = 1,
    
    // Enables normal mapping
    SF_NormalMap = 2,
    
    // Enables specular highlights
    SF_Specular = 4,
    
    // Enables AlphaTest cutout transparency
    SF_Cutout = 8,
    
    // Enables fog effects
    SF_Fog = 16,
    
    // Visualization of shadow maps
    SF_Debug_ShadowMapTexture = 32,
    
    // Visualization of depth textures
    SF_Debug_DepthTexture = 64,
    
    // Visualization of cascade split distances
    SF_Debug_ShowCascadeSplits = 128,
    
    // Visualization of the voxel tree traversal depth
    SF_Debug_ShowVoxelTreeDepth = 512,
    
    // Enables voxel PCF filtering
    SF_Shadow_PCF_Filter = 1024,
};


// Encodes a combination of shader features.
typedef unsigned int ShaderFeatureList;


// Manages a single variant of a shader.
class Shader
{
public:
    Shader(const string &name, ShaderFeatureList features);
    ~Shader();
    
    // Prevent shaders from being copied
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Allow a shader to be moved
    Shader(Shader&& other);
    Shader& operator=(Shader&& other);
    
    // Feature management
    ShaderFeatureList features() const { return features_; }
    bool hasFeature(ShaderFeature feature) const;
    
    // Program and shader ids
    GLuint program() const { return program_; }
    
    void bind();
    
private:
    ShaderFeatureList features_;
    GLuint program_;
    
    // Shader compilation
    bool compileShader(GLenum type, const char* file, GLuint &id);
    bool checkShaderErrors(GLuint shaderID);
    bool checkLinkerErrors(GLuint programID);
    
    // Sets a uniform block binding
    void setUniformBlockBinding(const char* blockName, GLuint id);
    
    // Sets the texture id for the named texture
    void setTextureBinding(const char* textureName, GLint id);
    
    // Creates a #define list for the enabled features
    string createFeatureDefines() const;
};
