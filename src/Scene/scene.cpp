#include "Scene.h"

#include <utility>

GameObject& Scene::addGameObject(std::unique_ptr<GameObject> object)
{
    GameObject& reference = *object;
    gameObjects.push_back(std::move(object));
    return reference;
}

const std::vector<std::unique_ptr<GameObject>>& Scene::getGameObjects() const
{
    return gameObjects;
}

void Scene::clearLevelObjects()
{
    std::erase_if(gameObjects, [](const auto& object)
    {
        return !object->persistentAcrossScenes;
    });
}
