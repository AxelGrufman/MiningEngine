#pragma once

#include <GLFW/glfw3.h>

#include "Rendering/Renderer.h"
#include "Rendering/Shader.h"
#include "Assets/MeshManager.h"
#include "Input/InputSystem.h"
#include "Input/InputMappingManager.h"

struct EngineContext
{
    GLFWwindow* window = nullptr;

    Renderer renderer;
    Shader shader;
    MeshManager meshManager;
    InputSystem input;
    InputMappingManager inputMappings;
};