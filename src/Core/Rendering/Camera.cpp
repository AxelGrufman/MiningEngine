#include "Camera.h"

#include "Transform.h"

#include  <glm/gtc/matrix_transform.hpp>



glm::mat4 Camera::getViewMatrix(const Transform& transform) const
{
    glm::mat4 cameraWorld(1.0f);

    cameraWorld = glm::translate(cameraWorld, transform.position);

    cameraWorld = glm::rotate(cameraWorld, transform.rotation.z, glm::vec3(0, 0, 1));
    cameraWorld = glm::rotate(cameraWorld, transform.rotation.y, glm::vec3(0, 1, 0));
    cameraWorld = glm::rotate(cameraWorld, transform.rotation.x, glm::vec3(1, 0, 0));

    return glm::inverse(cameraWorld);
}

void Camera::setProjectionMode(ProjectionMode mode)
{
    projectionMode = mode;
}

void Camera::setAspectRatio(float aspect)
{
    if (aspect > 0.0f)
    {
        aspectRatio = aspect;
    }
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