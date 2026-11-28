#include "Scene.h"

#include "ECS/Components/NameComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/LevelObjectTag.h"

void Scene::clearLevelObjects()
{
    world.delete_with<LevelObjectTag>();
}

flecs::world& Scene::getWorld()
{
    return world;
}
const flecs::world& Scene::getWorld() const
{
    return world;
}

flecs::entity Scene::createEntity(const std::string& name)
{
    flecs::entity entity = world.entity();

    entity.set<NameComponent>({name});
    entity.set<TransformComponent>({});

    return entity;
}