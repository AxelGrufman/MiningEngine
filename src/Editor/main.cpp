#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <filesystem>

#include "Engine.h"
#include "Rendering/shader.h"
#include "Rendering/Camera.h"
#include "Rendering/Renderer.h"
#include "Scene/scene.h"
#include "Rendering/mesh.h"
#include "Rendering/Renderable.h"
#include "Assets/MeshManager.h"
#include "EngineContext.h"
#include "Logic/Controllers/FlyingCameraController.h"
#include "Scene/CameraObject.h"
#include "Scene/SceneLoader.h"
#include "Scene/SceneBuilder.h"

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
	editor.bindKey("LoadScene1", GLFW_KEY_1);
	editor.bindKey("LoadScene2", GLFW_KEY_2);
	editor.bindKey("LoadScene3", GLFW_KEY_3);
	
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
	int requestedScene = 0; 
	
	Camera editorCamera;
	Transform editorCameraTransform;
	editorCameraTransform.position = {0.0f, 3.0f, 13.5f};
	
	CameraObject& player1Object = scene.createGameObject<CameraObject>();
	player1Object.transform.position = {0.0f, 1.0f, 5.0f};
	player1Object.persistentAcrossScenes = true;
	
	CameraObject& player2Object = scene.createGameObject<CameraObject>();
	player2Object.transform.position = {2.0f, 1.0f, 5.0f};
	player2Object.persistentAcrossScenes = true;

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
	
	context.inputMappings.subscribe("Editor", "LoadScene1", InputPhase::Started, [&](const InputActionEvent&)
	{
		requestedScene = 1;
	});

	context.inputMappings.subscribe("Editor", "LoadScene2", InputPhase::Started, [&](const InputActionEvent&)
	{
		requestedScene = 2;
	});

	context.inputMappings.subscribe("Editor", "LoadScene3", InputPhase::Started, [&](const InputActionEvent&)
	{
		requestedScene = 3;
	});
	
	//Player 1
	context.inputMappings.subscribe( "Player1", "Look", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyLook(player1Object.transform, event.value);
	});
	
	context.inputMappings.subscribe("Player1", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyMovement(player1Object.transform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player1", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		playerCameraController.applyVerticalMovement(player1Object.transform, event.value.x, deltaTime);
	});
	
	
	//Player 2
	context.inputMappings.subscribe("Player2", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyMovement(player2Object.transform, event.value, deltaTime);
	});
    
	context.inputMappings.subscribe("Player2", "KeyboardLook", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyKeyboardLook(player2Object.transform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player2", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		player2CameraController.applyVerticalMovement(player2Object.transform, event.value.x, deltaTime);
	});
	//---------------------------------------
	
	if (!context.shader.load( "assets/shaders/basic3d.vert", "assets/shaders/basic.frag"))
	{
		cleanup(window, context.shader);
		return -1;
	}
	
	auto sceneData = SceneLoader::load("assets/Scenes/scene1.mscene");

	if (sceneData)
	{
		SceneBuilder::addWorldObjects(scene, *sceneData, context.meshManager, context.textureManager);

		player1Object.transform.position = sceneData->player1Start;
		player1Object.transform.rotation = glm::radians(sceneData->player1Rotation);

		player2Object.transform.position = sceneData->player2Start;
		player2Object.transform.rotation = glm::radians(sceneData->player2Rotation);

		std::cout << "Loaded scene: " << sceneData->name << '\n';
	}
	else
	{
		std::cerr << "Failed to load scene!\n";
	}
	
	Mesh* markerMesh = context.meshManager.loadMesh("assets/models/Editormodels/MatineeCam_SM.obj");
	if (markerMesh)
	{
		player1Object.renderable.emplace();
		player1Object.renderable->mesh = markerMesh;
		player1Object.renderable->localScale = {0.02f, 0.02f, 0.02f};
		player1Object.renderable->localRotation = {0.0f, glm::radians(90.0f), 0.0f};
		
		player2Object.renderable.emplace();
		player2Object.renderable->mesh = markerMesh;
		player2Object.renderable->localScale = {0.02f, 0.02f, 0.02f};
		player2Object.renderable->localRotation = {0.0f, glm::radians(90.0f), 0.0f};
	}
	
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
		
		if (requestedScene != 0)
		{
			const std::string scenePaths[] = {"assets/Scenes/scene1.mscene", "assets/Scenes/scene2.mscene", "assets/Scenes/scene3.mscene"};
			auto data = SceneLoader::load(scenePaths[requestedScene - 1]);

			if (data && !data->name.empty())
			{
				scene.clearLevelObjects();
				SceneBuilder::addWorldObjects(scene, *data, context.meshManager, context.textureManager);

				player1Object.transform.position = data->player1Start;
				player1Object.transform.rotation = glm::radians(data->player1Rotation);

				player2Object.transform.position = data->player2Start;
				player2Object.transform.rotation = glm::radians(data->player2Rotation);

				std::cout << "Switched to scene: " << data->name << '\n';
			}
			else
			{
				std::cerr << "Failed to switch scene!\n";
			}
			requestedScene = 0;
		}
		
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
			player1Object.camera.setAspectRatio(static_cast<float>(width) / topHeight);
			context.renderer.renderScene(scene, player1Object.camera, player1Object.transform, context.shader, {&player1Object});
			
			// Player 2 - Bottom
			glViewport(0, 0, width, bottomHeight);
			player2Object.camera.setAspectRatio(static_cast<float>(width) / bottomHeight);
			context.renderer.renderScene(scene, player2Object.camera, player2Object.transform, context.shader, {&player2Object});
		}
		else
		{
			glViewport(0, 0, width, height);
			bool isEditor = engine.getMode() == EngineMode::Editor;

			Camera& activeCamera = isEditor ? editorCamera : player1Object.camera;
			const Transform& activeTransform = isEditor ? editorCameraTransform : player1Object.transform;
			const GameObject* excludedObject = isEditor ? nullptr : &player1Object;

			activeCamera.setAspectRatio(static_cast<float>(width) / height);
			context.renderer.renderScene(scene, activeCamera, activeTransform, context.shader, {&player1Object, &player2Object});
		}

		glfwSwapBuffers(window);
	}
	context.textureManager.destroy();
	context.meshManager.destroy();
	cleanup(window, context.shader);
	return 0;
}
