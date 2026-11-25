#pragma once

#include <optional>
#include <string>

#include "SceneData.h"

class SceneLoader
{
public:
    static std::optional<SceneData> load(const std::string& filePath);
};