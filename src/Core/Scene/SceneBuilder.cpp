#include "SceneBuilder.h"

#include "Scene.h"
#include "SceneData.h"
#include "GameObject.h"
#include "Assets/MeshManager.h"
#include "Assets/TextureManager.h"

#include <glm/trigonometric.hpp>
#include <iostream>

void SceneBuilder::addWorldObjects(Scene& scene, const SceneData& data, MeshManager& meshManager, TextureManager& textureManager)
{
    for (const SceneObjectData& objectData : data.objects)
    {
        Mesh* mesh = meshManager.loadMesh(objectData.meshPath);

        if (!mesh)
        {
            std::cerr << "[SceneBuilder] Warning: Failed to load mesh for " << objectData.name << ". Skipping object.\n";
            continue;
        }

        GameObject& object = scene.createGameObject<GameObject>();

        object.transform.position = objectData.position;
        object.transform.rotation = glm::radians(objectData.rotation);
        object.transform.scale = objectData.scale;

        object.renderable.emplace();
        object.renderable->mesh = mesh;
        object.renderable->texture = textureManager.loadTexture(objectData.texturePath);
    }
}