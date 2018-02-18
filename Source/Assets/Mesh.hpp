#pragma once

#include <vector>

#include "Vector3.hpp"

typedef unsigned short MeshElementIndex;

class Mesh
{
public:
    Mesh(std::vector<Vector3> &positions, int elementsCount, const void* elementsOffset, int baseVertex);
    
    // Object space vertex positions
    const Vector3* vertices() const { return &positions_[0]; }
    int verticesCount() const { return (int)positions_.size(); }
    
    // Elements info
    int elementsCount() const { return elementsCount_; }
    const void* elementsOffset() const { return elementsOffset_; }
    int baseVertex() const { return baseVertex_; }
    
private:
    std::vector<Vector3> positions_;
    int elementsCount_;
    const void* elementsOffset_;
    int baseVertex_;
};
