#include "MeshCollection.hpp"

#include <string>
#include <fstream>
#include <cstdio>

Mesh* MeshCollection::fullScreenQuad_;

MeshCollection::MeshCollection()
    : positions_(),
    normals_(),
    tangents_(),
    texcoords_(),
    elements_()
{
    // Create the full screen quad instance
    addFullScreenQuad();
    
    // Create the vertex array
    glGenVertexArrays(1, &vertexArray_);
    
    // Create vertex attribute buffers for the vao
    glGenBuffers(4, vertexBuffers_);
    
    // Create an elements buffer
    glGenBuffers(1, &elementsBuffer_);
}

MeshCollection::~MeshCollection()
{
    glDeleteBuffers(1, &elementsBuffer_);
    glDeleteBuffers(4, vertexBuffers_);
    glDeleteVertexArrays(1, &vertexArray_);
}

Mesh* MeshCollection::load(const char* fileName)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<Vector2> texcoords;
    std::vector<MeshElementIndex> elements;
    
    std::ifstream file(fileName);
    
    // Read lines from the mesh file
    while(file.is_open() && !file.fail() && !file.eof())
    {
        string type;
        file >> type;
        
        if(type == "vertex")
        {
            Vector3 v;
            file >> v;
            positions.push_back(v);
        }
        else if(type == "normal")
        {
            Vector3 n;
            file >> n;
            normals.push_back(n);
        }
        else if(type == "tangent")
        {
            Vector4 t;
            file >> t;
            tangents.push_back(t);
        }
        else if(type == "texcoord")
        {
            Vector2 t;
            file >> t;
            texcoords.push_back(t);
        }
        else if(type == "triangle")
        {
            MeshElementIndex a, b, c;
            file >> a >> b >> c;
            elements.push_back(c);
            elements.push_back(b);
            elements.push_back(a);
        }
        else
        {
            printf("Unknown mesh type %s in file %s \n", type.c_str(), fileName);
            return NULL;
        }
    }
    
    // Check for errors
    if(file.fail())
    {
        printf("Error reading mesh file %s \n", fileName);
        return NULL;
    }
    
    return addMesh(positions, normals, tangents, texcoords, elements);
}

void MeshCollection::upload()
{
    glBindVertexArray(vertexArray_);
    
    // Positions buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * positions_.size(), &positions_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normals buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * normals_.size(), &normals_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(1);
    
    // Tangents buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * tangents_.size(), &tangents_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 4, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(2);
    
    // Texcoords buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers_[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2) * texcoords_.size(), &texcoords_[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 2, GL_FLOAT, false, 0, (void*)0);
    glEnableVertexAttribArray(3);
    
    // Elements buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsBuffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(MeshElementIndex) * elements_.size(), &elements_[0], GL_STATIC_DRAW);
	
    // The cpu copy is no longer needed
    positions_.clear();
    positions_.shrink_to_fit();
    normals_.clear();
    normals_.shrink_to_fit();
    tangents_.clear();
    tangents_.shrink_to_fit();
    texcoords_.clear();
    texcoords_.shrink_to_fit();
    elements_.clear();
    elements_.shrink_to_fit();
}

Mesh* MeshCollection::addMesh(std::vector<Vector3> newPositions,
                                    std::vector<Vector3> newNormals,
                                    std::vector<Vector4> newTangents,
                                    std::vector<Vector2> newTexcoords,
                                    std::vector<MeshElementIndex> newElements)
{
    // Add the attributes to the relevent lists.
    positions_.insert(positions_.end(), newPositions.begin(), newPositions.end());
    normals_.insert(normals_.end(), newNormals.begin(), newNormals.end());
    tangents_.insert(tangents_.end(), newTangents.begin(), newTangents.end());
    texcoords_.insert(texcoords_.end(), newTexcoords.begin(), newTexcoords.end());
    
    // Insert the elements to the end of the elements list
    elements_.insert(elements_.end(), newElements.begin(), newElements.end());
    
    // Finally, return a mesh that references the correct location
    const int elementsCount = (int)newElements.size();
    const void* elementsOffset = (void*)((elements_.size() - newElements.size()) * sizeof(MeshElementIndex));
    const int baseVertex = (int)(positions_.size() - newPositions.size());
    return new Mesh(newPositions, elementsCount, elementsOffset, baseVertex);
}

void MeshCollection::addFullScreenQuad()
{
    // Create attribute lists
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector4> tangents;
    std::vector<Vector2> texcoords;
    std::vector<MeshElementIndex> elements;
    
    // 4 vertices are needed
    positions.push_back(Vector3(1.0, 1.0, 1.0));
    positions.push_back(Vector3(1.0, -1.0, 1.0));
    positions.push_back(Vector3(-1.0, 1.0, 1.0));
    positions.push_back(Vector3(-1.0, -1.0, 1.0));
    
    for(int i = 0; i < 4; ++i)
    {
        normals.push_back(Vector3::zero());
        tangents.push_back(Vector4(0.0, 0.0, 0.0, 0.0));
        texcoords.push_back(Vector2(0.0, 0.0));
    }
    
    // First triangle (lower right)
    elements.push_back(0);
    elements.push_back(3);
    elements.push_back(1);
    
    // Second triangle (upper left)
    elements.push_back(0);
    elements.push_back(2);
    elements.push_back(3);
    
    // Create mesh
    fullScreenQuad_ = addMesh(positions, normals, tangents, texcoords, elements);
}
