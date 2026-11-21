#include "Renderer.h"

#include "Scene/scene.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

void Renderer::renderScene(const Scene& scene, const Camera& camera, const Transform& cameraTransform, Shader& shader) const
{
    shader.use();
    
    glUniform1i(shader.getUniformLocation("uTexture"),0);
    glUniform3f(shader.getUniformLocation("uColor"),0.4f, 0.8f, 1.0f);
    glUniform3f(shader.getUniformLocation("uLightDirection"),0.0f, 0.0f, 3.0f);

    glm::mat4 view = camera.getViewMatrix(cameraTransform);
    glm::mat4 projection = camera.getProjectionMatrix();

    glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));

    for (const auto& renderable : scene.getRenderables())
    {
        if (renderable.mesh == nullptr)
            continue;
        
        glm::mat4 model = renderable.transform.getModelMatrix();
        
        glUniformMatrix4fv(shader.getUniformLocation("uModel"),1, GL_FALSE, glm::value_ptr(model));
       
        bool hasTexture = renderable.texture != nullptr;
        glUniform1i(shader.getUniformLocation("uUseTexture"), hasTexture ? 1 : 0);
        if (hasTexture)
        {
            renderable.texture->bind();
        }
        
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderable.mesh->drawIndexed();
    }
}