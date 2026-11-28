#include "Renderer.h"

#include "Scene/scene.h"
#include "Shader.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TextureComponent.h"
#include "ECS/Components/TransformComponent.h"


#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

static void drawRenderable(Shader& shader, Mesh* mesh, Texture* texture, const glm::mat4& model)
{
    if (!mesh)
        return;

    glUniformMatrix4fv(shader.getUniformLocation("uModel"), 1, GL_FALSE, glm::value_ptr(model));

    bool hasTexture = texture != nullptr;
    glUniform1i(shader.getUniformLocation("uUseTexture"), hasTexture ? 1 : 0);

    if (hasTexture)
        texture->bind();

    mesh->drawIndexed();
}

void Renderer::renderScene(const Scene& scene, const CameraComponent& camera, const TransformComponent& cameraTransform, Shader& shader, std::initializer_list<flecs::entity> excludedEntities) const
{
    shader.use();
    
    glUniform1i(shader.getUniformLocation("uTexture"),0);
    glUniform3f(shader.getUniformLocation("uColor"),0.4f, 0.8f, 1.0f);
    glUniform3f(shader.getUniformLocation("uLightDirection"),0.0f, 0.0f, 3.0f);

    glm::mat4 view = camera.getViewMatrix(cameraTransform);
    glm::mat4 projection = camera.getProjectionMatrix();

    glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));
    
    scene.getWorld().each([&](flecs::entity entity, const TransformComponent& transform, const MeshComponent& mesh)
    {
        if (std::find(excludedEntities.begin(), excludedEntities.end(), entity) != excludedEntities.end())
            return;
        
        Texture* texture = nullptr;
        
        if (entity.has<TextureComponent>())
        {
            texture = entity.get<TextureComponent>().texture;
        }

        glm::mat4 model = transform.getModelMatrix();

        model = glm::rotate(model, mesh.localRotation.z, glm::vec3(0, 0, 1));
        model = glm::rotate(model, mesh.localRotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, mesh.localRotation.x, glm::vec3(1, 0, 0));
        model = glm::scale(model, mesh.localScale);

        drawRenderable(shader, mesh.mesh, texture, model);
    });
}

void Renderer::renderCameraMarkers(const Scene& scene, const CameraComponent& camera, const TransformComponent& cameraTransform, Shader& shader, Mesh* markerMesh, flecs::entity excludedEntity) const
{
    if (!markerMesh)
        return;

    shader.use();

    glm::mat4 view = camera.getViewMatrix(cameraTransform);
    glm::mat4 projection = camera.getProjectionMatrix();

    glUniformMatrix4fv(shader.getUniformLocation("uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(shader.getUniformLocation("uProjection"), 1, GL_FALSE, glm::value_ptr(projection));

    scene.getWorld().each([&](flecs::entity entity, const TransformComponent& transform, const CameraComponent&)
    {
        if (entity == excludedEntity)
            return;

        glm::mat4 model = transform.getModelMatrix();

        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0, 1, 0));
        model = glm::scale(model, glm::vec3(0.02f));

        drawRenderable(shader, markerMesh, nullptr, model);
    });
}