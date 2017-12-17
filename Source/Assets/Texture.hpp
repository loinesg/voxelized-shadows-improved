#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

using namespace std;

class Texture
{
public:
    Texture(GLuint id, int width, int height, GLint internalFormat, GLenum format, GLenum type);
    ~Texture();
    
    GLuint id() const { return id_; }
    int width() const { return width_; }
    int height() const { return height_; }
    
    // Basic texture settings
    void setWrapMode(GLint horizontal, GLint vertical);
    void setMinFilter(GLint filter);
    void setMagFilter(GLint filter);
    
    // Recreates the texture with a new resolution.
    void setResolution(int width, int height);
    
    // For depth textures.
    // Sets the texture comparison mode and function
    void setCompareMode(GLenum mode, GLenum func);
    
    // Generates mipmaps for the texture
    void generateMipmaps();
    
    // Binds the texture to the specified target
    void bind(GLenum target);
    
    // Loads a texture from a file.
    static Texture* load(const char* fileName);
    
    // Creates a depth texture.
    static Texture* depth(int width, int height);
    
    // Creates a texture for storing normalized linear depth
    static Texture* linearDepth(int width, int height);
    
    // Creates a texture with a single colour channel.
    static Texture* singleChannel(int width, int height);
    
private:
    GLuint id_;
    int width_;
    int height_;
    GLint internalFormat_;
    GLenum format_;
    GLenum type_;
};
