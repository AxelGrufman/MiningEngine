#pragma once

#include <glm/glm.hpp>

struct Transform;

class Camera
{
public:
    enum class ProjectionMode
    {
        Perspective,
        Orthographic
    };

    glm::mat4 getViewMatrix(const Transform& transform) const;
    glm::mat4 getProjectionMatrix() const;

    void setProjectionMode(ProjectionMode mode);
    void setAspectRatio(float aspect);

private:

    ProjectionMode projectionMode = ProjectionMode::Perspective;

    float aspectRatio = 800.0f / 600.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

	float fieldOfView = 45.0f;
	float orthographicSize = 3.5f;
};