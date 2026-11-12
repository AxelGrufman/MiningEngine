#include "Scene.h"

void Scene::addRenderable(const Renderable& renderable)
{
    renderables.push_back(renderable);
}

const std::vector<Renderable>& Scene::getRenderables() const
{
    return renderables;
}