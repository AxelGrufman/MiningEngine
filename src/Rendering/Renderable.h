#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "Texture.h"

struct Renderable
{
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
    Transform transform;
};