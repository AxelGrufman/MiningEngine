#pragma once

#include <optional>

#include "Rendering/Transform.h"
#include "Rendering/Renderable.h"

class GameObject
{
public:
    virtual ~GameObject() = default;

    Transform transform;
    std::optional<Renderable> renderable;
    bool persistentAcrossScenes = false;
};