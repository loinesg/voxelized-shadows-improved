#pragma once

#include <vector>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "Camera.hpp"
#include "Light.hpp"
#include "MeshCollection.hpp"
#include "MeshInstance.hpp"
#include "Object.hpp"
#include "Texture.hpp"
#include "Animation.hpp"

class Scene
{
public:
    Scene();
    
    // The viewers camera
    const Camera* mainCamera() const { return &cameras_[0]; }
    Camera* mainCamera() { return &cameras_[0]; }
    
    // The shadow casting light
    const Light* mainLight() const { return &lights_[0]; }
    
    // The mesh instances to be rendered
    const vector<MeshInstance>& meshInstances() const { return meshInstances_; }
    
    void update(float deltaTime);
    
    // Loads scene objects from the given .scene file
    bool loadFromFile(const string &fileName);
    
private:
    
    // Scene objects
    vector<Camera> cameras_;
    vector<Light> lights_;
    vector<MeshInstance> meshInstances_;
    vector<Animation> animations_;
    
    // Assets
    map<string, Mesh*> meshes_;
    map<string, Texture*> textures_;
    
    // Scene loading
    bool loadObject(ifstream &file);
    void loadObjectTransform(ifstream &file, Object* object);
    bool loadCamera(ifstream &file);
    bool loadLight(ifstream &file);
    bool loadMeshInstance(ifstream &file);
    bool loadAnimation(ifstream &file);
    
    // Used to store meshes before they are sent to the gpu
    MeshCollection meshCollection_;
    
    // Asset loading
    Mesh* getMesh(const string &name);
    Texture* getTexture(const string &name);
};
