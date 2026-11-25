#include "Renderer.h"

#include "Scene/scene.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

void Renderer::renderScene(const Scene& scene, const Camera& camera, const Transform& cameraTransform, Shader& shader, std::initializer_list<const GameObject*> excludedObjects) const
{
    shader.use();
    
    glUniform1i(shader.getUniformLocation("uTexture"),0);
    glUniform3f(shader.getUniformLocation("uColor"),0.4f, 0.8f, 1.0f);
    glUniform3f(shader.getUniformLocation("uLightDirection"),0.0f, 0.0f, 3.0f);

    glm::mat4 view = camera.getViewMatrix(cameraTransform);
    glm::mat4 projection = camera.getProjectionMatrix();

    glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));

    auto drawRenderable = [&](const Renderable& renderable, const glm::mat4& model)
    {
        if (!renderable.mesh)
            return;

        glUniformMatrix4fv(shader.getUniformLocation("uModel"), 1, GL_FALSE, glm::value_ptr(model));

        bool hasTexture = renderable.texture != nullptr;
        glUniform1i(shader.getUniformLocation("uUseTexture"), hasTexture ? 1 : 0);

        if (hasTexture)
            renderable.texture->bind();

        renderable.mesh->drawIndexed();
    };
    
    for (const auto& object : scene.getGameObjects())
    {
        if (std::find(excludedObjects.begin(), excludedObjects.end(), object.get()) != excludedObjects.end())
            continue;

        if (!object->renderable)
            continue;

        glm::mat4 model = object->transform.getModelMatrix();

        model = glm::rotate(model, object->renderable->localRotation.z, glm::vec3(0, 0, 1));
        model = glm::rotate(model, object->renderable->localRotation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, object->renderable->localRotation.x, glm::vec3(1, 0, 0));

        model = glm::scale(model, object->renderable->localScale);

        drawRenderable(*object->renderable, model);
    }
}