#pragma once

#include <glm/vec2.hpp>

struct Transform;

class FlyingCameraController
{
public:
    void applyLook(Transform& transform, const glm::vec2& lookDelta);
    void applyKeyboardLook(Transform& transform, const glm::vec2& lookInput, float deltaTime);
    
    void applyMovement(Transform& transform, const glm::vec2& moveInput, float deltaTime);
    void applyVerticalMovement(Transform& transform, float verticalInput, float deltaTime);

private:
    float mouseSensitivity = 0.002f;
    float moveSpeed = 5.0f;
};