#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>

#include "Rendering/shader.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Scene/scene.h"
#include "Assets/OBJLoader.h"
#include "Rendering/mesh.h"
#include "Rendering/Renderable.h"

void cleanup(GLFWwindow* window, Shader& shader)
{
	shader.destroy();

	
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

int main()
{
	GLFWwindow* window = createWindow(800, 600, "MiningEngine");
	if (!window)
		return -1;
	
	Shader shader;
	Camera camera;
	Scene scene;
	Renderer renderer;
	
		if (!shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, shader);
		return -1;
	}
	
	MeshData modelData;
	Mesh quadsphereMesh;

	if (OBJLoader::load("assets/models/triangelateTestobject.obj", modelData))
	{
		quadsphereMesh.setupMesh(modelData.vertices, modelData.indices);
		
		Renderable quadsphere{};
		quadsphere.mesh = &quadsphereMesh;
		
		quadsphere.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
		quadsphere.transform.scale = glm::vec3(0.1f);
		
		scene.addRenderable(quadsphere);

		std::cout << "Quadsphere added to scene!\n";
	}
	else
	{
		std::cerr << "Could not load Quadsphere!\n";
	}
	
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
		
		renderer.renderScene(scene, camera, shader);

		
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << error << '\n';
		}
			
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	cleanup(window, shader);
	return 0;
}
