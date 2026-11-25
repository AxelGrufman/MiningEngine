#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Rendering/mesh.h"

class MeshManager
{
public:
    Mesh* loadMesh(const std::string& filepath);
    void destroy();
    
private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
};