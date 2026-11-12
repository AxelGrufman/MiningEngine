#pragma once

#include "Mesh.h"
#include "Transform.h"

struct Renderable
{
    Mesh* mesh = nullptr;
    Transform transform;
};