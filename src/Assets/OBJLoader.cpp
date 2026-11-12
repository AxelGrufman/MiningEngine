#include "OBJLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <cstddef>
#include <exception>

bool OBJLoader::load(const std::string& filepath, MeshData& output)
{
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    
    MeshData data;
    
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << filepath << '\n';
        return false;
    }

   std::string line;
    
    while (std::getline(file, line))
    {
        size_t commentPos = line.find('#');

        if (commentPos != std::string::npos)
        {
            line.erase(commentPos);
        }
        
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        
        if (prefix == "o")
        {
            ss >> data.name;
        }
        else if (prefix == "v")
        {
            glm::vec3 pos{0.0f};

            if (!(ss >> pos.x >> pos.y >> pos.z))
            {
                std::cerr << "Invalid vertex position!\n";
                return false;
            }

            temp_positions.push_back(pos);
        }
        else if (prefix == "vt")
        {
            glm::vec2 uv{0.0f};

            if (!(ss >> uv.x >> uv.y))
            {
                std::cerr << "Invalid UV coordinates!\n";
                return false;
            }

            temp_uvs.push_back(uv);
        }
        else if (prefix == "vn")
        {
            glm::vec3 normal{0.0f};

            if (!(ss >> normal.x >> normal.y >> normal.z))
            {
                std::cerr << "Invalid normal position!\n";
                return false;
            }

            temp_normals.push_back(normal);
        }
        else if (prefix == "f")
        {
            std::vector<std::string> faceTokens;
            std::string token;
            
            while (ss >> token)
            {
                faceTokens.push_back(token);
            }
            
            auto resolveIndex = [](const std::string& indexStr,std::size_t vectorSize, std::size_t& result) -> bool
            {
                try
                {
                    std::size_t charactersRead = 0;
                    long long objIndex = std::stoll(indexStr, &charactersRead);
                    
                    if (charactersRead != indexStr.size() || objIndex == 0)
                    {
                        return false;
                    }

                    long long cppIndex;

                    if (objIndex > 0)
                    {
                        cppIndex = objIndex - 1;
                    }
                    else
                    {
                        cppIndex = static_cast<long long>(vectorSize) + objIndex;
                    }
                    
                    if (cppIndex < 0 ||
                        static_cast<std::size_t>(cppIndex) >= vectorSize)
                    {
                        return false;
                    }

                    result = static_cast<std::size_t>(cppIndex);
                    return true;
                }
                catch (const std::exception&)
                {
                    return false;
                }
            };

            std::vector<Vertex> faceVertices;
            
            for (const std::string& faceToken : faceTokens)
            {
                if (std::count(faceToken.begin(), faceToken.end(), '/') > 2 ||
                    faceToken.back() == '/')
                {
                    std::cerr << "Invalid OBJ face token: " << faceToken << '\n';
                    return false;
                }

                std::stringstream tokenStream(faceToken);

                std::string positionIndexStr;
                std::string uvIndexStr;
                std::string normalIndexStr;

                std::getline(tokenStream, positionIndexStr, '/');
                std::getline(tokenStream, uvIndexStr, '/');
                std::getline(tokenStream, normalIndexStr, '/');

                Vertex vertex{};
                std::size_t positionIndex;

                if (!resolveIndex(positionIndexStr, temp_positions.size(), positionIndex))
                {
                    std::cerr << "Invalid OBJ position index: "<< positionIndexStr << '\n';
                    return false;
                }

                vertex.position = temp_positions[positionIndex];
                
                if (!uvIndexStr.empty())
                {
                    std::size_t uvIndex;

                    if (!resolveIndex(uvIndexStr, temp_uvs.size(), uvIndex))
                    {
                        std::cerr << "Invalid OBJ UV index: "<< uvIndexStr << '\n';
                        return false;
                    }

                    vertex.uv = temp_uvs[uvIndex];
                }
                
                if (!normalIndexStr.empty())
                {
                    std::size_t normalIndex;

                    if (!resolveIndex(normalIndexStr, temp_normals.size(), normalIndex))
                    {
                        std::cerr << "Invalid OBJ normal index: "<< normalIndexStr << '\n';
                        return false;
                    }
                    
                    vertex.normal = temp_normals[normalIndex];
                }
                
                faceVertices.push_back(vertex);
            }
            
            if (faceVertices.size() != 3 && faceVertices.size() != 4)
            {
                std::cerr << "Unsupported OBJ face vertex count: "<< faceVertices.size() << '\n';
                return false;
            }
            
            unsigned int baseIndex =static_cast<unsigned int>(data.vertices.size());
            
            data.vertices.insert(data.vertices.end(),faceVertices.begin(),faceVertices.end());
            
            data.indices.push_back(baseIndex + 0);
            data.indices.push_back(baseIndex + 1);
            data.indices.push_back(baseIndex + 2);
            
            if (faceVertices.size() == 4)
            {
                data.indices.push_back(baseIndex + 0);
                data.indices.push_back(baseIndex + 2);
                data.indices.push_back(baseIndex + 3);
            }
        }
    }
    
    output = std::move(data);
    return true;
}