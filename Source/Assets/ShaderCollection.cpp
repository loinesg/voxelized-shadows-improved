#include "ShaderCollection.hpp"

ShaderCollection::ShaderCollection(const string &name)
    : shaderName_(name),
    supportedFeatures_(~0),
    enabledFeatures_(~0),
    shaderVariants_()
{
    
}

ShaderFeatureList ShaderCollection::enabledFeatures() const
{
    // Only include features that are both enabled and supported
    return enabledFeatures_ & supportedFeatures_;
}

void ShaderCollection::enableFeature(ShaderFeature feature)
{
    enabledFeatures_ |= feature;
}

void ShaderCollection::disableFeature(ShaderFeature feature)
{
    enabledFeatures_ &= ~feature;
}

void ShaderCollection::setSupportedFeatures(ShaderFeatureList supportedFeatures)
{
    supportedFeatures_ = supportedFeatures;
}

void ShaderCollection::bindVariant(ShaderFeatureList features)
{
    // Only use features that are enabled and supported.
    features &= (enabledFeatures_ & supportedFeatures_);
    
    // Try to find a cached shader.
    for(unsigned int i = 0; i < shaderVariants_.size(); ++i)
    {
        Shader& shader = shaderVariants_[i];
        if(shader.features() == features)
        {
            shader.bind();
            return;
        }
    }

    // No cached shader. Create a new shader.
    shaderVariants_.push_back(Shader(shaderName_, features));
    shaderVariants_.back().bind();
}
