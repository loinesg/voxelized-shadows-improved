#include "RenderPass.hpp"

#include "MeshCollection.hpp"

RenderPass::RenderPass(const string &name, UniformManager* uniformManager)
    : name_(name),
    clearFlags_(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT),
    clearColor_(PassClearColor(0.0, 0.0, 0.0, 1.0)),
    shaderCollection_(new ShaderCollection(name)),
    uniformManager_(uniformManager)
{
    
}

RenderPass::~RenderPass()
{
    delete shaderCollection_;
}

void RenderPass::setClearFlags(PassClearFlags clearFlags)
{
    clearFlags_ = clearFlags;
}

void RenderPass::setClearColor(PassClearColor color)
{
    clearColor_ = color;
}

void RenderPass::enableFeature(ShaderFeature feature)
{
    shaderCollection_->enableFeature(feature);
}

void RenderPass::disableFeature(ShaderFeature feature)
{
    shaderCollection_->disableFeature(feature);
}

void RenderPass::setSupportedFeatures(ShaderFeatureList supportedFeatures)
{
    shaderCollection_->setSupportedFeatures(supportedFeatures);
}

void RenderPass::submit(Camera* camera, const vector<MeshInstance> &instances, bool drawStatic, bool drawDynamic)
{
    // Setup the camera uniform buffer
    CameraUniformBuffer cub;
    cub.screenResolution = Vector4(camera->pixelWidth(), camera->pixelHeight(), 0.0, 0.0);
    cub.cameraPosition = Vector4(camera->position(), 1.0);
    cub.worldToView = camera->worldToLocal();
    cub.viewProjection = camera->worldToCameraMatrix();
    cub.clipToWorld = camera->cameraToWorldMatrix();
    
    // If we are in perspective mode, also set up the frustum corners
    if(camera->type() == CameraType::Perspective)
    {
        camera->getFrustumCorners(camera->farPlane(), cub.frustumCorners);
        
        // Transform the results to a world space vector of length 1
        for(int i = 0; i < 4; ++i)
        {
            cub.frustumCorners[i] = Vector4(camera->localToWorldVector(cub.frustumCorners[i].vec3()), 0.0f);
        }
        
        // Specify the clip plane distances
        cub.cameraClipPlanes.x = camera->nearPlane();
        cub.cameraClipPlanes.y = camera->farPlane();
    }
    
    uniformManager_->updateCameraBuffer(cub);
    
    // Clear the screen
    if(clearFlags_ > 0)
    {
        glClearColor(clearColor_.x, clearColor_.y, clearColor_.z, clearColor_.w);
        glClear(clearFlags_);
    }
    
    // Remember the previously used state
    // Used to skip needless state changes
    ShaderFeatureList prevShaderFeatures = ~0;
    Texture* prevTexture = NULL;
    Texture* prevNormalMap = NULL;
    Mesh* prevMesh = NULL;
    
    // Try to group instances into a single batch
    PerObjectUniformBuffer instanceData;
    int instanceCount = 0;
    
    ShaderFeatureList enabledFeatures = shaderCollection_->enabledFeatures();
    for(auto instance = instances.begin(); instance != instances.end(); instance++)
    {
        // Check if the instance should be skipped due to its static flag
        if((instance->isStatic() && !drawStatic)
           || (!instance->isStatic() && !drawDynamic))
        {
            continue;
        }
        
        ShaderFeatureList shaderFeatures = instance->shaderFeatures() & enabledFeatures;
        Texture* texture = instance->texture();
        Texture* normalMap = instance->normalMap();
        Mesh* mesh = instance->mesh();
        Matrix4x4 transform = instance->localToWorld();
        
        // Check if anything is different to the previous mesh
        if(shaderFeatures != prevShaderFeatures
           || texture != prevTexture
           || normalMap != prevNormalMap
           || mesh != prevMesh)
        {
            // Send the queued instances
            if(instanceCount > 0)
            {
                uniformManager_->updatePerObjectBuffer(instanceData, instanceCount);
                glDrawElementsInstancedBaseVertex(GL_TRIANGLES, prevMesh->elementsCount(), GL_UNSIGNED_SHORT, prevMesh->elementsOffset(), instanceCount, prevMesh->baseVertex());
                instanceCount = 0;
            }
            
            // Bind the correct shader
            if(shaderFeatures != prevShaderFeatures)
                shaderCollection_->bindVariant(shaderFeatures);
                
            // Bind the correct main texture
            if(texture != prevTexture
               && (shaderFeatures & (SF_Texture | SF_Cutout)) != 0)
                texture->bind(GL_TEXTURE0);
            
            // Bind the correct normal map texture
            if(normalMap != prevNormalMap
               && (shaderFeatures & SF_NormalMap) != 0)
                normalMap->bind(GL_TEXTURE1);
        }
        
        // Add this mesh to the queue
        instanceData.localToWorld[instanceCount] = transform;
        instanceCount ++;
        
        prevShaderFeatures = shaderFeatures;
        prevTexture = texture;
        prevNormalMap = normalMap;
        prevMesh = mesh;
    }
    
    // Send any remaining queued instances
    if(instanceCount > 0)
    {
        uniformManager_->updatePerObjectBuffer(instanceData, instanceCount);
        glDrawElementsInstancedBaseVertex(GL_TRIANGLES, prevMesh->elementsCount(), GL_UNSIGNED_SHORT, prevMesh->elementsOffset(), instanceCount, prevMesh->baseVertex());
    }
}

void RenderPass::renderFullScreen()
{
    // Use all supported shader features
    shaderCollection_->bindVariant(~0);
    
    // Use a full screen quad
    const Mesh* mesh = MeshCollection::fullScreenQuad();
    
    // Draw the quad
    glDrawElementsBaseVertex(GL_TRIANGLES, mesh->elementsCount(), GL_UNSIGNED_SHORT, mesh->elementsOffset(), mesh->baseVertex());
}
