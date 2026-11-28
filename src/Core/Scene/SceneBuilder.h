#pragma once

class Scene;
struct SceneData;
class MeshManager;
class TextureManager;

class SceneBuilder
{
public:
    static void buildEntities(Scene& scene, const SceneData& data, MeshManager& meshManager, TextureManager& textureManager);
};