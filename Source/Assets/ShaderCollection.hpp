#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <string>
#include <vector>

using namespace std;

#include "Shader.hpp"

// Manages loading and binding variants of a shader.
class ShaderCollection
{
public:
    ShaderCollection(const string &name);
    
    // The currently enabled features
    ShaderFeatureList enabledFeatures() const;
    
    // Feature toggling
    void enableFeature(ShaderFeature feature);
    void disableFeature(ShaderFeature feature);
    void setSupportedFeatures(ShaderFeatureList supportedFeatures);
    
    // Finds and loading a shader variant
    void bindVariant(ShaderFeatureList features);
    
private:
    string shaderName_;
    ShaderFeatureList supportedFeatures_;
    ShaderFeatureList enabledFeatures_;
    vector<Shader> shaderVariants_;
};
