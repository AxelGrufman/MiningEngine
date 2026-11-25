#pragma once

#include <glm/vec3.hpp>

#include "Mesh.h"
#include "Texture.h"


struct Renderable
{
    Mesh* mesh = nullptr;
    Texture* texture = nullptr;
    glm::vec3 localScale{1.0f};
    glm::vec3 localRotation{0.0f};
};