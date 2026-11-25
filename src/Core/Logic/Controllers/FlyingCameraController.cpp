#include "FlyingCameraController.h"
#include "Rendering/Transform.h"

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

void FlyingCameraController::applyLook(Transform& transform, const glm::vec2& lookDelta)
{
    transform.rotation.y -= lookDelta.x * mouseSensitivity;
    transform.rotation.x -= lookDelta.y * mouseSensitivity;

    transform.rotation.x = glm::clamp(transform.rotation.x, glm::radians(-89.0f), glm::radians(89.0f));
}

void FlyingCameraController::applyKeyboardLook(Transform& transform, const glm::vec2& lookInput, float deltaTime)
{
    float turnSpeed = 1.8f;

    transform.rotation.y -= lookInput.x * turnSpeed * deltaTime;
    transform.rotation.x -= lookInput.y * turnSpeed * deltaTime;

    transform.rotation.x = glm::clamp(transform.rotation.x, glm::radians(-89.0f), glm::radians(89.0f));
}

void FlyingCameraController::applyMovement(Transform& transform, const glm::vec2& moveInput, float deltaTime)
{
    float yaw = transform.rotation.y;
    float pitch = transform.rotation.x;

    glm::vec3 forward = { -glm::sin(yaw) * glm::cos(pitch), glm::sin(pitch), -glm::cos(yaw) * glm::cos(pitch)};
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 direction = forward * moveInput.y + right * moveInput.x;

    if (glm::length(direction) > 0.0f)
    {
        direction = glm::normalize(direction);
        transform.position += direction * moveSpeed * deltaTime;
    }
}

void FlyingCameraController::applyVerticalMovement(Transform& transform, float verticalInput, float deltaTime)
{
    transform.position.y += verticalInput * moveSpeed * deltaTime;
}