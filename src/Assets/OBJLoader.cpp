#include "OBJLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <cstddef>
#include <exception>
#include <map>
#include <tuple>
#include <limits>
#include <cmath>
#include <numeric>
#include <glm/glm.hpp>

 double OBJLoader::cross2D(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c)
    {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    }

bool OBJLoader::triangulateFace(const std::vector<unsigned int>& faceIndices, const std::vector<Vertex>& vertices, std::vector<unsigned int>& outputIndices)
    {
        const std::size_t count = faceIndices.size();

        if (count < 3)
            return false;
     
        glm::dvec3 normal(0.0);

        for (std::size_t i = 0; i < count; ++i)
        {
            glm::dvec3 a(vertices[faceIndices[i]].position);
            glm::dvec3 b(vertices[faceIndices[(i + 1) % count]].position);

            normal.x += (a.y - b.y) * (a.z + b.z);
            normal.y += (a.z - b.z) * (a.x + b.x);
            normal.z += (a.x - b.x) * (a.y + b.y);
        }

        if (glm::dot(normal, normal) == 0.0)
            return false;
     
        glm::dvec3 absNormal(
            std::abs(normal.x),
            std::abs(normal.y),
            std::abs(normal.z)
        );

        std::vector<glm::dvec2> points;
        points.reserve(count);

        for (unsigned int index : faceIndices)
        {
            glm::dvec3 p(vertices[index].position);

            if (absNormal.x >= absNormal.y &&
                absNormal.x >= absNormal.z)
            {
                points.emplace_back(p.y, p.z);
            }
            else if (absNormal.y >= absNormal.z)
            {
                points.emplace_back(p.x, p.z);
            }
            else
            {
                points.emplace_back(p.x, p.y);
            }
        }
     
        double minX = points[0].x;
        double maxX = points[0].x;
        double minY = points[0].y;
        double maxY = points[0].y;

        for (const auto& p : points)
        {
            minX = std::min(minX, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
        }

        double scale = std::max(maxX - minX, maxY - minY);

        if (scale == 0.0)
            return false;

        const double epsilon = 1e-12 * scale * scale;
     
        std::vector<std::size_t> remaining;

        for (std::size_t i = 0; i < count; ++i)
        {
            if (!remaining.empty())
            {
                glm::dvec2 difference =
                    points[i] - points[remaining.back()];

                if (glm::dot(difference, difference) <= epsilon)
                    continue;
            }

            remaining.push_back(i);
        }
     
        if (remaining.size() > 1)
        {
            glm::dvec2 difference =
                points[remaining.front()] - points[remaining.back()];

            if (glm::dot(difference, difference) <= epsilon)
                remaining.pop_back();
        }

        if (remaining.size() < 3)
            return false;
     
        double signedArea = 0.0;

        for (std::size_t i = 0; i < remaining.size(); ++i)
        {
            const auto& a = points[remaining[i]];
            const auto& b = points[remaining[(i + 1) % remaining.size()]];

            signedArea += a.x * b.y - b.x * a.y;
        }

        if (std::abs(signedArea) <= epsilon)
            return false;

        const double winding = signedArea > 0.0 ? 1.0 : -1.0;
     
        std::vector<unsigned int> triangles;
     
        while (remaining.size() > 3)
        {
            bool foundEar = false;

            for (std::size_t i = 0; i < remaining.size(); ++i)
            {
                std::size_t previous =remaining[(i + remaining.size() - 1) % remaining.size()];

                std::size_t current = remaining[i];

                std::size_t next =remaining[(i + 1) % remaining.size()];

                const auto& a = points[previous];
                const auto& b = points[current];
                const auto& c = points[next];

                if (winding * cross2D(a, b, c) <= epsilon)
                    continue;

                bool containsOtherVertex = false;

                for (std::size_t other : remaining)
                {
                    if (other == previous || other == current || other == next)
                    {
                        continue;
                    }

                    const auto& p = points[other];

                    bool inside = winding * cross2D(a, b, p) >= -epsilon && winding * cross2D(b, c, p) >= -epsilon && winding * cross2D(c, a, p) >= -epsilon;

                    if (inside)
                    {
                        containsOtherVertex = true;
                        break;
                    }
                }

                if (containsOtherVertex)
                    continue;
                
                triangles.push_back(faceIndices[previous]);
                triangles.push_back(faceIndices[current]);
                triangles.push_back(faceIndices[next]);
                
                remaining.erase(remaining.begin() + i);

                foundEar = true;
                break;
            }
            
            if (!foundEar)
                return false;
        }
     
        const auto& a = points[remaining[0]];
        const auto& b = points[remaining[1]];
        const auto& c = points[remaining[2]];

        if (winding * cross2D(a, b, c) <= epsilon)
        {
            return false;
        }
     
        triangles.push_back(faceIndices[remaining[0]]);
        triangles.push_back(faceIndices[remaining[1]]);
        triangles.push_back(faceIndices[remaining[2]]);
     
        outputIndices.insert( outputIndices.end(), triangles.begin(),triangles.end());

        return true;
    }


bool OBJLoader::load(const std::string& filepath, MeshData& output)
{
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;
    
    MeshData data;
    std::map<std::tuple<std::size_t, std::size_t, std::size_t>, unsigned int> vertexLookup;
    
    std::size_t reusedVertices = 0;
     std::size_t lineNumber = 0;
    
    std::ifstream file(filepath);

    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << filepath << '\n';
        return false;
    }

   std::string line;
    
    while (std::getline(file, line))
    {
        ++lineNumber;
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
            
            if (faceTokens.size() < 3)
            {
                std::cerr << "OBJ face must have at least 3 vertices!\n";
                return false;
            }

            std::vector<unsigned int> faceIndices;
            
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
                constexpr std::size_t missingIndex = std::numeric_limits<std::size_t>::max();

                std::size_t positionIndex;
                std::size_t uvIndex = missingIndex;
                std::size_t normalIndex = missingIndex;

                if (!resolveIndex(positionIndexStr, temp_positions.size(), positionIndex))
                {
                    std::cerr << "Invalid OBJ position index: "<< positionIndexStr << '\n';
                    return false;
                }

                vertex.position = temp_positions[positionIndex];
                
                if (!uvIndexStr.empty())
                {
                    if (!resolveIndex(uvIndexStr, temp_uvs.size(), uvIndex))
                    {
                        std::cerr << "Invalid OBJ UV index: "<< uvIndexStr << '\n';
                        return false;
                    }

                    vertex.uv = temp_uvs[uvIndex];
                }
                
                if (!normalIndexStr.empty())
                {
                    if (!resolveIndex(normalIndexStr, temp_normals.size(), normalIndex))
                    {
                        std::cerr << "Invalid OBJ normal index: "<< normalIndexStr << '\n';
                        return false;
                    }
                    
                    vertex.normal = temp_normals[normalIndex];
                }
                
                auto key = std::make_tuple(positionIndex, uvIndex, normalIndex);
                auto it = vertexLookup.find(key);
                unsigned int vertexIndex;

                if (it != vertexLookup.end())
                {
                    vertexIndex = it->second;
                    ++reusedVertices;
                }
                else
                {
                    vertexIndex = static_cast<unsigned int>(data.vertices.size());
                    data.vertices.push_back(vertex);
                    vertexLookup.emplace(key, vertexIndex);
                }
                
                faceIndices.push_back(vertexIndex);
            }
            
            if (faceIndices.size() == 3)
            {
                data.indices.push_back(faceIndices[0]);
                data.indices.push_back(faceIndices[1]);
                data.indices.push_back(faceIndices[2]);
            }
            else if (faceIndices.size() == 4)
            {
                data.indices.push_back(faceIndices[0]);
                data.indices.push_back(faceIndices[1]);
                data.indices.push_back(faceIndices[2]);

                data.indices.push_back(faceIndices[0]);
                data.indices.push_back(faceIndices[2]);
                data.indices.push_back(faceIndices[3]);
            }
            else
            {
                if (!triangulateFace(faceIndices, data.vertices, data.indices))
                {
                    std::cerr << "Warning: Could not triangulate an n-gon. " << "Skipping this face.\n";
                }
            }
        }
    }
    std::cout << "Reused vertices: " << reusedVertices << '\n';
    output = std::move(data);
    return true;
}