#pragma once

#define GL_GLEXT_PROTOTYPES 1 // Enables OpenGL 3 Features
#include <QGLWidget> // Links OpenGL Headers

#include <vector>

#include "Mesh.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

typedef unsigned short MeshElementIndex;

class MeshCollection
{
public:
    MeshCollection();
    ~MeshCollection();
    
    // Prevent the collection from being copied
    MeshCollection(const MeshCollection&) = delete;
    MeshCollection& operator=(const MeshCollection&) = delete;
    
    // Adds a fullscreen quad to the collection's buffer
    static Mesh* fullScreenQuad() { return fullScreenQuad_; }
    
    // Loads a mesh from a file and adds it to the collection's buffer
    Mesh* load(const char* fileName);
    
    // Uploads the contents of the attribute and elements buffers to the gpu
    // Once this is done, no more meshes can be made.
    void upload();
    
private:
    // The vertex attributes and elements in the mesh collection
    // This will be cleared once the collection is on the gpu
    std::vector<Vector3> positions_;
    std::vector<Vector3> normals_;
    std::vector<Vector4> tangents_;
    std::vector<Vector2> texcoords_;
    std::vector<MeshElementIndex> elements_;
    
    // The full screen quad mesh in the collection
    static Mesh* fullScreenQuad_;
    
    // The opengl vao and buffers
    GLuint vertexArray_;
    GLuint vertexBuffers_[4];
    GLuint elementsBuffer_;
    
    Mesh* addMesh(std::vector<Vector3> newPositions,
                 std::vector<Vector3> newNormals,
                 std::vector<Vector4> newTangents,
                 std::vector<Vector2> newTexcoords,
                 std::vector<MeshElementIndex> newElements);
    
    void addFullScreenQuad();
};
