#include "InputSystem.h"

#include <iostream>

void InputSystem::initialize(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, InputSystem::keyCallback);
    glfwSetMouseButtonCallback(window, InputSystem::mouseButtonCallback);
    glfwSetCursorPosCallback(window, InputSystem::cursorPositionCallback);
}

bool InputSystem::isKeyDown(int key) const
{
    if (key < 0 || key > GLFW_KEY_LAST)
    {
        return false;
    }
    return keysDown[key];
}

bool InputSystem::isMouseButtonDown(int button) const
{
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return false;

    return mouseButtonsDown[button];
}

void InputSystem::clearEvents()
{
    keyEvents.clear();
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
}

const std::vector<KeyEvent>& InputSystem::getKeyEvents() const
{
    return keyEvents;
}

double InputSystem::getMouseDeltaX() const
{
    return mouseDeltaX;
}

double InputSystem::getMouseDeltaY() const
{
    return mouseDeltaY;
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputSystem* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    
    if (input == nullptr)
    {
        return;
    }

    if (key < 0 || key > GLFW_KEY_LAST)
    {
        return;
    }
    
    if (action == GLFW_PRESS)
    {
        input->keysDown[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->keysDown[key] = false;
    }
    
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
    {
        input->keyEvents.push_back({key, action});
    }
}

void InputSystem::mouseButtonCallback( GLFWwindow* window, int button, int action, int mods)
{
    InputSystem* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (input == nullptr)
    {
        return;
    }

    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
    {
        return;
    }

    if (action == GLFW_PRESS)
    {
        input->mouseButtonsDown[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->mouseButtonsDown[button] = false;
    }
      
}

void InputSystem::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    InputSystem* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (input == nullptr)
        return;

    if (!input->hasMousePosition)
    {
        input->lastMouseX = xpos;
        input->lastMouseY = ypos;
        input->hasMousePosition = true;
        return;
    }

    input->mouseDeltaX += xpos - input->lastMouseX;
    input->mouseDeltaY += ypos - input->lastMouseY;

    input->lastMouseX = xpos;
    input->lastMouseY = ypos;
}