#pragma once

class Scene;
struct SceneData;
class MeshManager;
class TextureManager;

class SceneBuilder
{
public:
    static void addWorldObjects(Scene& scene, const SceneData& data, MeshManager& meshManager, TextureManager& textureManager);
};