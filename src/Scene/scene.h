#pragma once

#include <vector>
#include "Rendering/Renderable.h"
#include <memory>
#include "GameObject.h"

class Scene
{
public:
    GameObject& addGameObject(std::unique_ptr<GameObject> object);
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const;
    
    template<typename T> T& createGameObject()
    {
        auto object = std::make_unique<T>();
        T& reference = *object;

        addGameObject(std::move(object));

        return reference;
    }

private:
    std::vector<std::unique_ptr<GameObject>> gameObjects;
};