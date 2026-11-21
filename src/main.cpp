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
#include "Logic/Controllers/FlyingCameraController.h"

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
	Scene scene;
	Texture texture;
	context.window = window;
	context.input.initialize(window);

	// EDITOR
	InputMappingContext editor;
	editor.name = "Editor";
	editor.enabled = true;
	editor.priority = 10;

	editor.bindMouseButton("EditorCameraLook", GLFW_MOUSE_BUTTON_RIGHT);
	editor.bindMouseMotion("Look");
	editor.bindKey("Perspective", GLFW_KEY_P);
	editor.bindKey("Orthographic", GLFW_KEY_O);
	
	context.inputMappings.addContext(editor);
	
	// PLAYER 1
	InputMappingContext player1;
	player1.name = "Player1";
	player1.enabled = false;
	player1.priority = 0;
	
	player1.bindAxis2DKey("Move", GLFW_KEY_W, {0, 1});
	player1.bindAxis2DKey("Move", GLFW_KEY_S, {0, -1});
	player1.bindAxis2DKey("Move", GLFW_KEY_A, {-1, 0});
	player1.bindAxis2DKey("Move", GLFW_KEY_D, {1, 0});
	
	player1.bindMouseMotion("Look");
	
	player1.bindAxis1DKey("Vertical", GLFW_KEY_SPACE, 1.0f);
	player1.bindAxis1DKey("Vertical", GLFW_KEY_LEFT_CONTROL, -1.0f);
	
	context.inputMappings.addContext(player1);
	
	//Player 2
	InputMappingContext player2;
	player2.name = "Player2";
	player2.enabled = false;
	player2.priority = 0;

	player2.bindAxis2DKey("Move", GLFW_KEY_UP, {0, 1});
	player2.bindAxis2DKey("Move", GLFW_KEY_DOWN, {0, -1});
	player2.bindAxis2DKey("Move", GLFW_KEY_LEFT, {-1, 0});
	player2.bindAxis2DKey("Move", GLFW_KEY_RIGHT, {1, 0});
	
	player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_8, {0, -1});
	player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_2, {0, 1});
	player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_4, {-1, 0});
	player2.bindAxis2DKey("KeyboardLook", GLFW_KEY_KP_6, {1, 0});

	player2.bindAxis1DKey("Vertical", GLFW_KEY_RIGHT_SHIFT, 1.0f);
	player2.bindAxis1DKey("Vertical", GLFW_KEY_RIGHT_CONTROL, -1.0f);

	context.inputMappings.addContext(player2);
	
	float deltaTime = 0.0f;
	
	FlyingCameraController editorCameraController;
	FlyingCameraController playerCameraController;
	FlyingCameraController player2CameraController;
	bool editorLooking = false;
	bool splitScreenEnabled = false;
	
	Camera editorCamera;
	Transform editorCameraTransform;
	editorCameraTransform.position = {0.0f, 3.0f, 13.5f};
	
	Camera player1Camera;
	Transform playerCameraTransform;
	playerCameraTransform.position = {0.0f, 1.0f, 5.0f};
	
	Camera player2Camera;
	Transform player2CameraTransform;
	player2CameraTransform.position = {2.0f, 1.0f, 5.0f};

	//-----------Temp inputs subscribs -----
	
	//editor	
	context.inputMappings.subscribe("Editor", "Perspective", InputPhase::Started, [&](const InputActionEvent&)
	{
		editorCamera.setProjectionMode(Camera::ProjectionMode::Perspective);
	});

	context.inputMappings.subscribe("Editor","Orthographic", InputPhase::Started, [&](const InputActionEvent&)
	{
		editorCamera.setProjectionMode(Camera::ProjectionMode::Orthographic);
	});
	
	context.inputMappings.subscribe("Editor", "EditorCameraLook", InputPhase::Started, [&](const InputActionEvent&)
	{
		editorLooking = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		context.input.resetMouseTracking();
	});
	
	context.inputMappings.subscribe("Editor", "EditorCameraLook", InputPhase::Canceled, [&](const InputActionEvent&)
	{
		editorLooking = false;
		if (engine.getMode() == EngineMode::Editor)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			context.input.resetMouseTracking();
		}
	});
	
	context.inputMappings.subscribe("Editor", "Look", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (editorLooking)
		{
			editorCameraController.applyLook(editorCameraTransform, event.value);
		}
	});
	
	
	//Player 1
	context.inputMappings.subscribe( "Player1", "Look", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyLook(playerCameraTransform, event.value);
	});
	
	context.inputMappings.subscribe("Player1", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyMovement(playerCameraTransform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player1", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyVerticalMovement(playerCameraTransform, event.value.x, deltaTime);
	});
	
	
	//Player 2
	context.inputMappings.subscribe("Player2", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyMovement(player2CameraTransform, event.value, deltaTime);
	});
    
	context.inputMappings.subscribe("Player2", "KeyboardLook", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyKeyboardLook(player2CameraTransform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player2", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyVerticalMovement(player2CameraTransform, event.value.x, deltaTime);
	});
	//---------------------------------------
	
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
	
	renderObj("assets/models/Cube.obj",				 glm::vec3(-6.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f,  0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Monky.obj",				 glm::vec3(-3.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f,  0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/triangelateTestobject.obj",glm::vec3( 0.0f, -2.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.0f, 0.0f,  0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Sphere.obj",				 glm::vec3( 3.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -90.0f,0.0f), scene,context.meshManager, &texture);
	renderObj("assets/models/Torus.obj",				 glm::vec3( 6.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 00.0f, 0.0f), scene,context.meshManager, &texture);
		
	double lastFrameTime = glfwGetTime();
	
	while (engine.isRunning())
	{
		double currentTime = glfwGetTime();
		deltaTime = static_cast<float>(currentTime - lastFrameTime);
		lastFrameTime = currentTime;
		
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
					context.inputMappings.setContextEnabled("Player2", false);
					splitScreenEnabled = false;
				}
			}
			if (event.key == GLFW_KEY_F6 && event.action == GLFW_PRESS && engine.getMode() == EngineMode::Play)
			{
				splitScreenEnabled = !splitScreenEnabled;
				context.inputMappings.setContextEnabled("Player2", splitScreenEnabled);
				std::cout << "Split-screen: " << (splitScreenEnabled ? "ON" : "OFF") << '\n';
			}
		}


		context.inputMappings.process(context.input);
		context.input.clearEvents();
		
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		if (width <= 0 || height <= 0)
			continue;

		glClearColor(0.4f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (engine.getMode() == EngineMode::Play && splitScreenEnabled)
		{
			int bottomHeight = height / 2;
			int topHeight = height - bottomHeight;

			// Player 1 - Top
			glViewport(0, bottomHeight, width, topHeight);
			player1Camera.setAspectRatio(static_cast<float>(width) / topHeight);
			context.renderer.renderScene(scene, player1Camera, playerCameraTransform, context.shader);

			// Player 2 - Bottom
			glViewport(0, 0, width, bottomHeight);
			player2Camera.setAspectRatio(static_cast<float>(width) / bottomHeight);
			context.renderer.renderScene(scene, player2Camera, player2CameraTransform, context.shader);
		}
		else
		{
			glViewport(0, 0, width, height);

			bool isEditor = engine.getMode() == EngineMode::Editor;

			Camera& activeCamera = isEditor ? editorCamera : player1Camera;
			const Transform& activeTransform = isEditor ? editorCameraTransform : playerCameraTransform;

			activeCamera.setAspectRatio(static_cast<float>(width) / height);
			context.renderer.renderScene(scene, activeCamera, activeTransform, context.shader);
		}

		glfwSwapBuffers(window);
	}
	context.meshManager.destroy();
	texture.destroy();
	cleanup(window, context.shader);
	return 0;
}
