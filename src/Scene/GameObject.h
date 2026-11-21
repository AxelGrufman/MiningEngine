#pragma once

#include "Rendering/Transform.h"

class GameObject
{
public:
    virtual ~GameObject() = default;

    Transform transform;
};