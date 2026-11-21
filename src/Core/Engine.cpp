#include "Engine.h"

#include <iostream>

void Engine::enterPlayMode()
{
    if (currentMode == EngineMode::Play)
        return;

    currentMode = EngineMode::Play;
    glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    context.input.resetMouseTracking();
    std::cout << "Entered Play mode\n";
}

void Engine::stopPlayMode()
{
    if (currentMode == EngineMode::Editor)
        return;

    currentMode = EngineMode::Editor;
    glfwSetInputMode(context.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    context.input.resetMouseTracking();
    std::cout << "Returned to Editor mode\n";
}

bool Engine::isRunning() const
{
    return context.window != nullptr && !glfwWindowShouldClose(context.window);
}