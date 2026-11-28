#include <iostream>
#include <glad/gl.h>  
#include <GLFW/glfw3.h>
#include <string>
#include <flecs.h>

#include "Engine.h"
#include "Rendering/shader.h"
#include "ECS/Components/CameraComponent.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/PlayerComponent.h"
#include "Rendering/Renderer.h"
#include "Scene/scene.h"
#include "Assets/MeshManager.h"
#include "EngineContext.h"
#include "Logic/Controllers/FlyingCameraController.h"
#include "Scene/SceneLoader.h"
#include "Scene/SceneBuilder.h"

#include "Game.h"


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
	
	MiningGame::initialize(context.inputMappings);
	
	float deltaTime = 0.0f;
	
	FlyingCameraController editorCameraController;
	FlyingCameraController playerCameraController;
	FlyingCameraController player2CameraController;
	bool editorLooking = false;
	bool splitScreenEnabled = false;
	int requestedScene = 0;
	
	flecs::entity editorCameraEntity = scene.createEntity("EditorCamera");
	editorCameraEntity.set<CameraComponent>({});
	editorCameraEntity.get_mut<TransformComponent>().position = {0.0f, 3.0f, 13.5f};
	
	flecs::entity player1Entity;
	flecs::entity player2Entity;

	auto findPlayers = [&]()
	{
		player1Entity = flecs::entity{};
		player2Entity = flecs::entity{};

		scene.getWorld().each([&](flecs::entity entity, const PlayerComponent& player)
		{
			if (player.playerIndex == 1)
			{
				player1Entity = entity;
			}
			else if (player.playerIndex == 2)
			{
				player2Entity = entity;
			}
		});
	};
	
	auto isValidPlayer = [](flecs::entity entity) -> bool
	{
		return entity.is_alive() && entity.has<PlayerComponent>() && entity.has<TransformComponent>() && entity.has<CameraComponent>();
	};
	
	auto stopPlayMode = [&]()
	{
		engine.stopPlayMode();
		context.inputMappings.setContextEnabled("Editor", true);
		context.inputMappings.setContextEnabled("Player1", false);
		context.inputMappings.setContextEnabled("Player2", false);
		splitScreenEnabled = false;
	};
	
	//-----------Temp inputs subscribs -----
	
	//editor	
	context.inputMappings.subscribe("Editor", "Perspective", InputPhase::Started, [&](const InputActionEvent&)
	{
		editorCameraEntity.get_mut<CameraComponent>().setProjectionMode(CameraComponent::ProjectionMode::Perspective);
	});

	context.inputMappings.subscribe("Editor","Orthographic", InputPhase::Started, [&](const InputActionEvent&)
	{
		editorCameraEntity.get_mut<CameraComponent>().setProjectionMode(CameraComponent::ProjectionMode::Orthographic);
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
			auto& transform = editorCameraEntity.get_mut<TransformComponent>();

			editorCameraController.applyLook(transform, event.value);
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
		if (!isValidPlayer(player1Entity))
			return;
		
		auto& transform = player1Entity.get_mut<TransformComponent>();
		playerCameraController.applyLook(transform, event.value);
	});
	
	context.inputMappings.subscribe("Player1", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (!isValidPlayer(player1Entity))
			return;
		
		auto& transform = player1Entity.get_mut<TransformComponent>();
		playerCameraController.applyMovement(transform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player1", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (!isValidPlayer(player1Entity))
			return;
		
		auto& transform = player1Entity.get_mut<TransformComponent>();
		playerCameraController.applyVerticalMovement(transform, event.value.x, deltaTime);
	});
	
	
	//Player 2
	context.inputMappings.subscribe("Player2", "Move", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (!isValidPlayer(player2Entity))
			return;
		
		auto& transform = player2Entity.get_mut<TransformComponent>();
		player2CameraController.applyMovement(transform, event.value, deltaTime);
	});
    
	context.inputMappings.subscribe("Player2", "KeyboardLook", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (!isValidPlayer(player2Entity))
			return;
		
		auto& transform = player2Entity.get_mut<TransformComponent>();
		player2CameraController.applyKeyboardLook(transform, event.value, deltaTime);
	});
	
	context.inputMappings.subscribe("Player2", "Vertical", InputPhase::Performed, [&](const InputActionEvent& event)
	{
		if (!isValidPlayer(player2Entity))
			return;
		
		auto& transform = player2Entity.get_mut<TransformComponent>();
		player2CameraController.applyVerticalMovement(transform, event.value.x, deltaTime);
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
		SceneBuilder::buildEntities(scene, *sceneData, context.meshManager, context.textureManager);
		findPlayers();
		std::cout << "Loaded scene: " << sceneData->name << '\n';
	}
	else
	{
		std::cerr << "Failed to load scene!\n";
	}
	
	Mesh* cameraMarkerMesh = context.meshManager.loadMesh("assets/models/Editormodels/MatineeCam_SM.obj");
	
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
					if (!isValidPlayer(player1Entity))
					{
						std::cerr << "Cannot enter play mode: No valid Player 1!\n";
						continue;
					}
					
					engine.enterPlayMode();
					context.inputMappings.setContextEnabled("Editor", false);
					context.inputMappings.setContextEnabled("Player1", true);
				}
				else
				{
					stopPlayMode();
				}
			}
			if (event.key == GLFW_KEY_F6 && event.action == GLFW_PRESS && engine.getMode() == EngineMode::Play)
			{
				if (!splitScreenEnabled && !isValidPlayer(player2Entity))
				{
					std::cerr << "Cannot enable split-screen: No valid Player 2!\n";
					continue;
				}
				
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
				SceneBuilder::buildEntities(scene, *data, context.meshManager, context.textureManager);
				findPlayers();
				std::cout << "Switched to scene: " << data->name << '\n';
			}
			else
			{
				std::cerr << "Failed to switch scene!\n";
			}
			requestedScene = 0;
		}
		
		if (engine.getMode() == EngineMode::Play && !isValidPlayer(player1Entity))
		{
			stopPlayMode();
			std::cerr << "Play mode stopped: Player 1 is invalid!\n";
		}
		
		if (splitScreenEnabled && !isValidPlayer(player2Entity))
		{
			splitScreenEnabled = false;
			context.inputMappings.setContextEnabled("Player2", false);

			std::cerr << "Split-screen disabled: Player 2 is invalid!\n";
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
			auto& P1camera = player1Entity.get_mut<CameraComponent>();
			const auto& P1transform = player1Entity.get<TransformComponent>();
			P1camera.setAspectRatio(static_cast<float>(width) / topHeight);
			context.renderer.renderScene(scene, P1camera, P1transform, context.shader, {player1Entity});
			
			// Player 2 - Bottom
			glViewport(0, 0, width, bottomHeight);
			auto& P2camera = player2Entity.get_mut<CameraComponent>();
			const auto& P2transform = player2Entity.get<TransformComponent>();
			P2camera.setAspectRatio(static_cast<float>(width) / bottomHeight);
			context.renderer.renderScene(scene, P2camera, P2transform, context.shader, {player2Entity});
		}
		else
		{
			glViewport(0, 0, width, height);
			bool isEditor = engine.getMode() == EngineMode::Editor;

			flecs::entity activeEntity = isEditor ? editorCameraEntity : player1Entity;
			auto& activeCamera = activeEntity.get_mut<CameraComponent>();
			const auto& activeTransform = activeEntity.get<TransformComponent>();

			activeCamera.setAspectRatio(static_cast<float>(width) / height);
			if (isEditor)
			{
				context.renderer.renderScene(scene, activeCamera, activeTransform, context.shader);
				context.renderer.renderCameraMarkers(scene, activeCamera, activeTransform, context.shader, cameraMarkerMesh, editorCameraEntity);
			}
			else
			{
				context.renderer.renderScene(scene, activeCamera, activeTransform, context.shader, {player1Entity});
			}
		}

		glfwSwapBuffers(window);
	}
	context.textureManager.destroy();
	context.meshManager.destroy();
	cleanup(window, context.shader);
	return 0;
}
