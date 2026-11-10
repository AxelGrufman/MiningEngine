#include "Camera.h"

#include  <glm/gtc/matrix_transform.hpp>


glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, target, up);
}

void Camera::setProjectionMode(ProjectionMode mode)
{
    projectionMode = mode;
}

glm::mat4 Camera::getProjectionMatrix() const
{
    if (projectionMode == ProjectionMode::Perspective)
    {
        return glm::perspective(glm::radians(fieldOfView),aspectRatio,nearPlane,farPlane);
    }

    float halfWidth = orthographicSize * aspectRatio;

    return glm::ortho(-halfWidth,halfWidth,-orthographicSize,orthographicSize, nearPlane, farPlane);
}