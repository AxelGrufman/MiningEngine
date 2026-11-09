#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <numbers>
#include <random>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/shader.h"
#include "Rendering/mesh.h"
#include "Rendering/Camera.h"

struct ShaderUniforms
{
	GLint offset = -1;
	GLint scale = -1;
	GLint rotation = -1;
	GLint color = -1;
};

struct EngineResources
{
	Mesh triangle;
	Mesh pyramid;
	Mesh cube;
	Mesh sphere;
};

void cleanup(GLFWwindow* window, EngineResources& resources, Shader& shader)
{
	shader.destroy();
	resources.triangle.destroy();
	resources.pyramid.destroy();
	resources.cube.destroy();
	resources.sphere.destroy();

	if (window != nullptr)
		glfwDestroyWindow(window);

	glfwTerminate();
}

GLFWwindow* createWindow(int width, int height, const char* title)
{
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return nullptr;
	}	
	GLFWwindow* window;
	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	
	if (!window)
	{
		std::cerr << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	
	glfwMakeContextCurrent(window);
	
	if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD!\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return nullptr;
	}
	
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	return window;
}

ShaderUniforms get_uniforms(Shader& shader)
{
	ShaderUniforms uniforms;

	uniforms.offset = shader.getUniformLocation("uOffset");
	uniforms.scale = shader.getUniformLocation("uScale");
	uniforms.rotation = shader.getUniformLocation("uRotation");
	uniforms.color = shader.getUniformLocation("uColor");
	return uniforms;
}

void drawMesh(const Mesh& mesh, GLint modelLocation, const glm::mat4& model)
{
	glUniformMatrix4fv(modelLocation, 1,GL_FALSE, glm::value_ptr(model));
	mesh.drawIndexed();
}

int main()
{
	GLFWwindow* window = createWindow(800, 600, "MiningEngine");
	if (!window)
		return -1;

	EngineResources resources;
	Shader shader;
	Camera camera;

	if (!resources.triangle.createTriangle())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.pyramid.createPyramid())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.cube.createCube())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	if (!resources.sphere.createSphere())
	{
		cleanup(window, resources, shader);
		return -1;
	}
	

	if (!shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, resources, shader);
		return -1;
	}
	
	ShaderUniforms uniforms = get_uniforms(shader);
	
	while (!glfwWindowShouldClose(window))
	{		
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			camera.setProjectionMode(Camera::ProjectionMode::Perspective);
		}

		if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		{
			camera.setProjectionMode(Camera::ProjectionMode::Orthographic);
		}
		
		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		shader.use();
		
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		
		glUniformMatrix4fv(shader.getUniformLocation("uView"),1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader.getUniformLocation("uProjection"),1, GL_FALSE, glm::value_ptr(projection));
		GLint modelLocation = shader.getUniformLocation("uModel");
		glUniform3f(uniforms.color, 0.3f, 0.4f, 0.9f);
		
		float time = static_cast<float>(glfwGetTime());
		
		glm::mat4 cube1(1.0f);
		cube1 = glm::translate(cube1, glm::vec3(-2.0f, 1.3f, 0.0f));
		cube1 = glm::rotate(cube1, time, glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 cube2(1.0f);
		cube2 = glm::translate(cube2, glm::vec3(-2.0f, -1.3f, 0.0f));
		
		glm::mat4 Sphere1(1.0f);
		float movement = glm::sin(time);
		Sphere1 = glm::translate(Sphere1,glm::vec3(2.0f , 1.3f, 0.0f + movement * 5));

		
		glm::mat4 Sphere2(1.0f);
		Sphere2 = glm::translate(Sphere2, glm::vec3(2.0f, -1.3f, 0.0f));
		
		glm::mat4 pyramid1(1.0f);
		pyramid1 = glm::translate(pyramid1, glm::vec3(0.0f, 1.3f, 0.0f));
		float pyramidScale = 1.0f + 0.8f * glm::sin(time);
		pyramid1 = glm::scale(pyramid1,glm::vec3(pyramidScale, pyramidScale, pyramidScale));
		
		glm::mat4 pyramid2(1.0f);
		pyramid2 = glm::translate(pyramid2, glm::vec3(0.0f, -1.3f, 0.0f));
		
		
		drawMesh(resources.cube, modelLocation, cube1);
		drawMesh(resources.cube, modelLocation, cube2);
		drawMesh(resources.sphere, modelLocation, Sphere1);
		drawMesh(resources.sphere, modelLocation, Sphere2);
		drawMesh(resources.pyramid, modelLocation, pyramid1);
		drawMesh(resources.pyramid, modelLocation, pyramid2);
		

		
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << error << '\n';
		}
			
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanup(window, resources, shader);
	return 0;
}
