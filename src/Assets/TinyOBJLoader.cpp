
#include "TinyOBJLoader.h"

#include <tiny_obj_loader.h>
#include <filesystem>
#include <iostream>
#include <utility>

bool TinyOBJLoader::load(const std::string& filepath, MeshData& output)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warning;
    std::string error;
    
    std::string modelDirectory = std::filesystem::path(filepath).parent_path().string();

    if (!modelDirectory.empty())
    {
        modelDirectory += std::filesystem::path::preferred_separator;
    }
    
    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, filepath.c_str(), modelDirectory.empty() ? nullptr : modelDirectory.c_str(), true);

    if (!warning.empty())
    {
        std::cerr << "TinyOBJ warning: " << warning << '\n';
    }
    
    if (!success)
    {
        std::cerr << "TinyOBJ error: " << error << '\n';
        return false;
    }

    MeshData data;
    data.name = std::filesystem::path(filepath).stem().string();
    
    auto validIndex = [](int index, std::size_t stride, std::size_t arraySize) -> bool
    {
        return index >= 0 && static_cast<std::size_t>(index) < arraySize / stride;
    };

    for (const auto& shape : shapes)
    {
        std::size_t offset = 0;

        for (unsigned int corners : shape.mesh.num_face_vertices)
        {
            if (corners != 3 || offset + corners > shape.mesh.indices.size())
            {
                std::cerr << "TinyOBJ could not produce triangles.\n";
                return false;
            }

            for (unsigned int i = 0; i < corners; ++i)
            {
                const auto& idx = shape.mesh.indices[offset + i];

                if (!validIndex(idx.vertex_index, 3, attrib.vertices.size()))
                {
                    std::cerr << "Invalid TinyOBJ position index.\n";
                    return false;
                }

                Vertex vertex{};

                int p = idx.vertex_index * 3;

                vertex.position = glm::vec3( attrib.vertices[p], attrib.vertices[p + 1], attrib.vertices[p + 2]);

                if (idx.normal_index >= 0)
                {
                    if (!validIndex(idx.normal_index, 3, attrib.normals.size()))
                    {
                        return false;
                    }

                    int n = idx.normal_index * 3;

                    vertex.normal = glm::vec3(attrib.normals[n], attrib.normals[n + 1],attrib.normals[n + 2]);
                }

                if (idx.texcoord_index >= 0)
                {
                    if (!validIndex(idx.texcoord_index, 2, attrib.texcoords.size()))
                    {
                        return false;
                    }

                    int t = idx.texcoord_index * 2;

                    vertex.uv = glm::vec2(attrib.texcoords[t],attrib.texcoords[t + 1]);
                }

                data.indices.push_back(static_cast<unsigned int>(data.vertices.size()));

                data.vertices.push_back(vertex);
            }

            offset += corners;
        }
    }

    output = std::move(data);
    return true;
}
