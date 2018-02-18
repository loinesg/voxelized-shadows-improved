#include "UniformManager.hpp"

UniformManager::UniformManager()
{
    createBuffers();
}

UniformManager::~UniformManager()
{
    glDeleteBuffers(1, &perObjectBlockID_);
    glDeleteBuffers(1, &sceneBlockID_);
    glDeleteBuffers(1, &cameraBlockID_);
    glDeleteBuffers(1, &shadowBlockID_);
    glDeleteBuffers(1, &voxelBlockID_);
}

void UniformManager::updatePerObjectBuffer(const PerObjectUniformBuffer &buffer, int instanceCount)
{
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4x4) * instanceCount, &buffer, GL_DYNAMIC_DRAW);
}

void UniformManager::updateSceneBuffer(const SceneUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, sceneBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneUniformBuffer), &buffer, GL_DYNAMIC_DRAW);
}

void UniformManager::updateCameraBuffer(const CameraUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, cameraBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUniformBuffer), &buffer, GL_DYNAMIC_DRAW);
}

void UniformManager::updateShadowBuffer(const ShadowUniformBuffer &buffer)
{
    glBindBuffer(GL_UNIFORM_BUFFER, shadowBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShadowUniformBuffer), &buffer, GL_DYNAMIC_DRAW);
}

void UniformManager::updateVoxelBuffer(const void* data, int sizeBytes)
{
    glBindBuffer(GL_UNIFORM_BUFFER, voxelBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeBytes, data, GL_STATIC_DRAW);
}

void UniformManager::createBuffers()
{
    // Per object buffer
    glGenBuffers(1, &perObjectBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, perObjectBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PerObjectUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, PerObjectUniformBuffer::BlockID, perObjectBlockID_);
    
    // Scene buffer
    glGenBuffers(1, &sceneBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, sceneBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SceneUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, SceneUniformBuffer::BlockID, sceneBlockID_);
    
    // Camera buffer
    glGenBuffers(1, &cameraBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, CameraUniformBuffer::BlockID, cameraBlockID_);
    
    // Shadow buffer
    glGenBuffers(1, &shadowBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, shadowBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(ShadowUniformBuffer), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, ShadowUniformBuffer::BlockID, shadowBlockID_);
    
    // Voxel buffer
    glGenBuffers(1, &voxelBlockID_);
    glBindBuffer(GL_UNIFORM_BUFFER, voxelBlockID_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(VoxelsUniformBuffer), NULL, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, VoxelsUniformBuffer::BlockID, voxelBlockID_);
    
    // Unbind
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
