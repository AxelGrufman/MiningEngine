#pragma once

#include <flecs.h>
#include <string>

class Scene
{
public:
    void clearLevelObjects();
    flecs::world& getWorld();
    const flecs::world& getWorld() const;
    flecs::entity createEntity(const std::string& name = "Unnamed Entity");
private:
    flecs::world world;
};