#pragma once

class Scene;
class CameraComponent;
class Shader;
struct TransformComponent;
class Mesh;

#include <flecs.h>
#include <initializer_list>

class Renderer
{
public:
    void renderScene(const Scene& scene, const CameraComponent& camera, const TransformComponent& cameraTransform, Shader& shader,std::initializer_list<flecs::entity> excludedEntities = {}) const;
    void renderCameraMarkers(const Scene& scene, const CameraComponent& camera, const TransformComponent& cameraTransform, Shader& shader, Mesh* markerMesh, flecs::entity excludedEntity) const;
};