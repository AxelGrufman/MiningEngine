#pragma once

#include <string>
#include "Rendering/MeshData.h"

class OBJLoader
{
public:
    static bool load(const std::string& filepath, MeshData& output);
};