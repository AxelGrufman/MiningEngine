#include "Scene.h"

#include <utility>


void Scene::addRenderable(const Renderable& renderable)
{
    renderables.push_back(renderable);
}
GameObject& Scene::addGameObject(std::unique_ptr<GameObject> object)
{
    GameObject& reference = *object;
    gameObjects.push_back(std::move(object));
    return reference;
}


const std::vector<Renderable>& Scene::getRenderables() const
{
    return renderables;
}