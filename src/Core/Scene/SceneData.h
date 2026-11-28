#pragma once

#include <string>
#include <vector>
#include <unordered_map>

struct SceneComponentData
{
    std::string type;
    std::unordered_map<std::string, std::string> properties;
};

struct SceneEntityData
{
    std::string name = "Unnamed Entity";
    std::vector<SceneComponentData> components;
};

struct SceneData
{
    std::string name;
    std::vector<SceneEntityData> entities;
};