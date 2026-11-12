#pragma once

#include <vector>
#include "vertex.h"

struct MeshData
{
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
};