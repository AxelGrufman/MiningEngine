#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
    enum class ProjectionMode
    {
        Perspective,
        Orthographic
    };

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setProjectionMode(ProjectionMode mode);

private:
    glm::vec3 position = { 0.0f, 0.0f, 10.0f };
    glm::vec3 target = { 0.0f, 0.0f, 0.0f };
    glm::vec3 up = { 0.0f, 1.0f, 0.0f };

    ProjectionMode projectionMode = ProjectionMode::Perspective;

    float aspectRatio = 800.0f / 600.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

	float fieldOfView = 45.0f;
	float orthographicSize = 3.5f;
};