#include "Mesh.hpp"

Mesh::Mesh(std::vector<Vector3> &positions, int elementsCount, const void* elementsOffset, int baseVertex)
    : positions_(positions),
    elementsCount_(elementsCount),
    elementsOffset_(elementsOffset),
    baseVertex_((int)baseVertex)
{
    
}
