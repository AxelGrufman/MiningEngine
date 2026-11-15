#include "MeshManager.h"
#include "OBJLoader.h"
#include "Assets/TinyOBJLoader.h"

#include <utility>
#include <iostream>
#include <filesystem>

Mesh* MeshManager::loadMesh(const std::string& filepath)
{
    std::string normalizedPath =std::filesystem::path(filepath).lexically_normal().generic_string();
    
    auto it = meshes.find(normalizedPath);

    if (it != meshes.end())
    {
        return it->second.get();
    }

    MeshData data;
    
    constexpr bool useTinyOBJ = true;
    
    
    if (useTinyOBJ)
    {
        std::cout << "using tiny obj loader" << std::endl;
        if (!TinyOBJLoader::load(filepath, data))
        {
            return nullptr;
        }
    }
    else
    {
        std::cout << "using own obj loader" << std::endl;
        if (!OBJLoader::load(filepath, data))
        {
            return nullptr;
        }
    }
    
    std::cout << "Loaded OBJ: " << filepath << '\n';
    std::cout << "Vertices: " << data.vertices.size() << '\n';
    std::cout << "Indices: " << data.indices.size() << '\n';
    
    if (data.vertices.empty() || data.indices.empty())
    {
        std::cerr << "OBJ contains no usable geometry: " << filepath << '\n';
        return nullptr;
    }

    auto mesh = std::make_unique<Mesh>();
    if (!mesh->setupMesh(data.vertices, data.indices))
    {
        std::cerr << "Failed to upload mesh: " << filepath << '\n';
        return nullptr;
    }

    Mesh* meshPtr = mesh.get();
    meshes.emplace(normalizedPath, std::move(mesh));

    return meshPtr;
}

void MeshManager::destroy()
{
    for (auto& [filepath, mesh] : meshes)
    {
        mesh->destroy();
    }

    meshes.clear();
}