#pragma once

class Scene;
class Camera;
class Shader;
struct Transform;

class Renderer
{
public:
    void renderScene(const Scene& scene, const Camera& camera, const Transform& cameraTransform, Shader& shader) const;
};