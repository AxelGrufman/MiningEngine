#include "SceneBuilder.h"

#include "Scene.h"
#include "SceneData.h"
#include "Assets/MeshManager.h"
#include "Assets/TextureManager.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TextureComponent.h"
#include "ECS/Components/LevelObjectTag.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/PlayerComponent.h"
#include "ECS/Components/GoalTag.h"

#include <glm/trigonometric.hpp>
#include <iostream>
#include <sstream>

static bool parseVec3(const std::string& text, glm::vec3& result)
{
    std::istringstream stream(text);
    char comma1, comma2;

    if (!(stream >> result.x >> comma1 >> result.y >> comma2 >> result.z))
        return false;

    if (comma1 != ',' || comma2 != ',')
        return false;

    stream >> std::ws;
    return stream.eof();
}

static glm::vec3 readVec3Property(const SceneComponentData& componentData, const std::string& key, glm::vec3 defaultValue)
{
    auto it = componentData.properties.find(key);

    if (it == componentData.properties.end())
        return defaultValue;

    glm::vec3 result{0.0f};

    if (!parseVec3(it->second, result))
    {
        std::cerr << "[SceneBuilder] Invalid property: " << componentData.type << "." << key << '\n';

        return defaultValue;
    }

    return result;
}

void SceneBuilder::buildEntities(Scene& scene, const SceneData& data, MeshManager& meshManager, TextureManager& textureManager)
{
    for (const SceneEntityData& entityData : data.entities)
    {
        flecs::entity entity = scene.createEntity(entityData.name);
        
        entity.add<LevelObjectTag>();
        
        for (const SceneComponentData& componentData : entityData.components)
        {
            if (componentData.type == "TransformComponent")
            {
                auto& transform = entity.get_mut<TransformComponent>();

                transform.position = readVec3Property(componentData, "position", glm::vec3{0.0f});
                transform.rotation = glm::radians(readVec3Property(componentData, "rotation", glm::vec3{0.0f}));
                transform.scale = readVec3Property(componentData, "scale", glm::vec3{1.0f});
            }
            else if (componentData.type == "MeshComponent")
            {
                auto it = componentData.properties.find("path");

                if (it == componentData.properties.end() || it->second.empty())
                {
                    std::cerr << "[SceneBuilder] Missing mesh path!\n";
                    continue;
                }

                Mesh* mesh = meshManager.loadMesh(it->second);

                if (!mesh)
                    continue;

                MeshComponent meshComponent;
                meshComponent.mesh = mesh;
                meshComponent.localScale = readVec3Property(componentData, "localScale", glm::vec3{1.0f});
                meshComponent.localRotation = glm::radians(readVec3Property(componentData, "localRotation", glm::vec3{0.0f}));

                entity.set<MeshComponent>(meshComponent);
            }
            else if (componentData.type == "TextureComponent")
            {
                auto it = componentData.properties.find("path");

                if (it == componentData.properties.end() || it->second.empty())
                {
                    std::cerr << "[SceneBuilder] Missing texture path!\n";
                    continue;
                }

                Texture* texture = textureManager.loadTexture(it->second);

                if (texture)
                {
                    entity.set<TextureComponent>({texture});
                }
            }
            else if (componentData.type == "PlayerComponent")
            {
                auto it = componentData.properties.find("playerIndex");

                if (it == componentData.properties.end())
                {
                    std::cerr << "[SceneBuilder] Missing player index!\n";
                    continue;
                }

                std::istringstream stream(it->second);
                int playerIndex = 0;

                if ((stream >> playerIndex) && (stream >> std::ws).eof() && playerIndex > 0)
                {
                    entity.set<PlayerComponent>({playerIndex});
                }
                else
                {
                    std::cerr << "[SceneBuilder] Invalid player index!\n";
                }
            }
            else if (componentData.type == "CameraComponent")
            {
                entity.set<CameraComponent>({});
            }
            else if (componentData.type == "GoalTag")
            {
                entity.add<GoalTag>();
            }
            else
            {
                std::cerr << "[SceneBuilder] Unknown component: " << componentData.type << '\n';
            }            
        }
    }
}