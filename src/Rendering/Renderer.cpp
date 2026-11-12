#include "Renderer.h"

#include "Scene/scene.h"
#include "Camera.h"
#include "Shader.h"

#include <glm/gtc/type_ptr.hpp>

void Renderer::renderScene(const Scene& scene, const Camera& camera, Shader& shader) const
{
    shader.use();
    
    
    glUniform3f(shader.getUniformLocation("uColor"),0.4f, 0.8f, 1.0f);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();

    glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));

    for (const auto& renderable : scene.getRenderables())
    {
        if (renderable.mesh == nullptr)
            continue;
        
        glm::mat4 model = renderable.transform.getModelMatrix();
        
        glUniformMatrix4fv(shader.getUniformLocation("uModel"),1, GL_FALSE, glm::value_ptr(model));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderable.mesh->drawIndexed();
    }
}