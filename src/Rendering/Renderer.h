#pragma once

class Scene;
class Camera;
class Shader;

class Renderer
{
public:
    void renderScene(const Scene& scene, const Camera& camera, Shader& shader) const;
    
};