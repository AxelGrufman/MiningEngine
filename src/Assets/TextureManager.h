#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "Rendering/Texture.h"

class TextureManager
{
public:
    Texture* loadTexture(const std::string& path);
    void destroy();

private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
};