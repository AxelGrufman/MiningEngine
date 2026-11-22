#pragma once

class Scene;
class Camera;
class Shader;
struct Transform;
class GameObject;

#include <initializer_list>

class Renderer
{
public:
    void renderScene(const Scene& scene, const Camera& camera, const Transform& cameraTransform, Shader& shader, std::initializer_list<const GameObject*> excludedObjects = {}) const;
};