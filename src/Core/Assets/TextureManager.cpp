#include "TextureManager.h"

#include <iostream>

Texture* TextureManager::loadTexture(const std::string& path)
{
    if (path.empty() || path == "none")
        return nullptr;

    auto it = textures.find(path);

    if (it != textures.end())
        return it->second.get();

    auto texture = std::make_unique<Texture>();

    if (!texture->load(path))
    {
        std::cerr << "[TextureManager] Warning: Failed to load " << path << '\n';
        return nullptr;
    }

    Texture* result = texture.get();

    textures.emplace(path, std::move(texture));

    return result;
}

void TextureManager::destroy()
{
    for (auto& [path, texture] : textures)
    {
        texture->destroy();
    }

    textures.clear();
}