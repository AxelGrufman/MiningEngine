#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>


#include "Rendering/shader.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Scene/scene.h"
#include "Rendering/mesh.h"
#include "Rendering/Renderable.h"
#include "Assets/MeshManager.h"
#include "Rendering/Texture.h"

bool renderObj(const std::string& name, const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rot, Scene& scene, MeshManager& meshManager, Texture* texture = nullptr)
{
	Mesh* mesh = meshManager.loadMesh(name);

	if (mesh == nullptr)
	{
		return false;
	}
	

	Renderable object{};
	object.mesh = mesh;
	object.transform.position = pos;
	object.transform.scale = scale;
	object.transform.rotation = glm::radians(rot);
	object.texture = texture;

	scene.addRenderable(object);

	return true;
}

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
	MeshManager meshManager;
	Texture texture;
	
	if (!shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, shader);
		return -1;
	}
	
	if (texture.load("assets/Texture/texturetest.png"))
	{
		std::cout << "Texture loaded successfully!\n";
	}
	else
	{
		std::cerr << "Texture loading failed!\n";
	}
	
	renderObj("assets/models/smaltestOBJFile.obj",glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), scene,meshManager, &texture);
	renderObj("assets/models/monky.obj",	      glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), scene,meshManager, &texture);
	renderObj("assets/models/testOBJFile.obj",    glm::vec3( 0.0f, -2.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), scene,meshManager, &texture);
	renderObj("assets/models/Turret.obj",         glm::vec3( 3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f), scene,meshManager, &texture);
	renderObj("assets/models/Knight.obj",		  glm::vec3( 6.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f), scene,meshManager, &texture);
	
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
	meshManager.destroy();
	texture.destroy();
	cleanup(window, shader);
	return 0;
}
