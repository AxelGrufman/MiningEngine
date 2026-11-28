#pragma once

#include <glm/vec2.hpp>

struct TransformComponent;

class FlyingCameraController
{
public:
    void applyLook(TransformComponent& transform, const glm::vec2& lookDelta);
    void applyKeyboardLook(TransformComponent& transform, const glm::vec2& lookInput, float deltaTime);
    
    void applyMovement(TransformComponent& transform, const glm::vec2& moveInput, float deltaTime);
    void applyVerticalMovement(TransformComponent& transform, float verticalInput, float deltaTime);

private:
    float mouseSensitivity = 0.002f;
    float moveSpeed = 5.0f;
};