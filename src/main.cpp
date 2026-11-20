#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>

#include "Core/Engine.h"
#include "Rendering/shader.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Scene/scene.h"
#include "Rendering/mesh.h"
#include "Rendering/Renderable.h"
#include "Assets/MeshManager.h"
#include "Rendering/Texture.h"
#include "Core/EngineContext.h"

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
	
	Engine engine;
	EngineContext& context = engine.getContext();
	context.window = window;
	context.input.initialize(window);
	
	InputMappingContext editorContext;
	editorContext.name = "Editor";
	editorContext.enabled = true;
	editorContext.mouseBindings.push_back({GLFW_MOUSE_BUTTON_RIGHT, "EditorCameraLook"});
	editorContext.mouseMotionBindings.push_back({"Look"});
	context.inputMappings.addContext(editorContext);
	
	InputMappingContext player1Context;
	player1Context.name = "Player1";
	player1Context.enabled = false;
	player1Context.keyBindings.push_back({GLFW_KEY_W, "MoveForward"});
	player1Context.mouseMotionBindings.push_back({"Look"});
	context.inputMappings.addContext(player1Context);
	
	Camera camera;
	Scene scene;
	Texture texture;
	
	
	if (!context.shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, context.shader);
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
	
	renderObj("assets/models/cube.obj",			  glm::vec3(-6.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/monky.obj",	      glm::vec3(-3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/triangelateTestobject.obj",    glm::vec3( 0.0f, -2.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f, 0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Turret.obj",         glm::vec3( 3.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Knight.obj",		  glm::vec3( 6.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f, 0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Cube.obj",				 glm::vec3(-6.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f,  0.0f), scene,meshManager, &texture);
	renderObj("assets/models/Monky.obj",				 glm::vec3(-3.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f,  0.0f), scene,meshManager, &texture);
	renderObj("assets/models/triangelateTestobject.obj",glm::vec3( 0.0f, -2.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f,  0.0f), scene,meshManager, &texture);
	renderObj("assets/models/Sphere.obj",				 glm::vec3( 3.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f,0.0f), scene,meshManager, &texture);
	renderObj("assets/models/Torus.obj",				 glm::vec3( 6.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 00.0f, 0.0f), scene,meshManager, &texture);
	
	while (engine.isRunning())
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
		
		context.renderer.renderScene(scene, camera, context.shader);

		
		GLenum error;
		while ((error = glGetError()) != GL_NO_ERROR)
		{
			std::cerr << "OpenGL error: " << error << '\n';
		}
			
		glfwSwapBuffers(window);
		glfwPollEvents();
		for (const KeyEvent& event : context.input.getKeyEvents())
		{
			if (event.key == GLFW_KEY_F5 && event.action == GLFW_PRESS)
			{
				if (engine.getMode() == EngineMode::Editor)
				{
					engine.enterPlayMode();
					context.inputMappings.setContextEnabled("Editor", false);
					context.inputMappings.setContextEnabled("Player1", true);
				}
				else
				{
					engine.stopPlayMode();
					context.inputMappings.setContextEnabled("Editor", true);
					context.inputMappings.setContextEnabled("Player1", false);
				}
			}
		}
		
		if (context.inputMappings.isActionDown("EditorCameraLook", context.input))
		{
			std::cout << "Editor camera look active\n";
		}
		if (context.inputMappings.isActionDown("MoveForward", context.input))
		{
			std::cout << "Player1 moving forward\n";
		}
		
		glm::vec2 look = context.inputMappings.getAxis2D("Look",context.input);

		if (look.x != 0.0f || look.y != 0.0f)
		{
			std::cout << "Mouse movement: "<< look.x << ", "<< look.y << '\n';
		}

		context.input.clearEvents();
	}
	context.meshManager.destroy();
	texture.destroy();
	cleanup(window, context.shader);
	return 0;
}
