#pragma once

#include "Rendering/mesh.h"
#include <glm/vec3.hpp>

struct MeshComponent
{
    Mesh* mesh = nullptr;

    glm::vec3 localScale{1.0f};
    glm::vec3 localRotation{0.0f};
};