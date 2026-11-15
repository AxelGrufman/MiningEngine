#pragma once

#include <string>
#include "Rendering/MeshData.h"


class OBJLoader
{
public:
    static bool load(const std::string& filepath, MeshData& output);
    
private:
    static bool triangulateFace(const std::vector<unsigned int>& faceIndices, const std::vector<Vertex>& vertices, std::vector<unsigned int>& outputIndices);
    static double cross2D(const glm::dvec2& a, const glm::dvec2& b, const glm::dvec2& c);
};