#include "InputSystem.h"

void InputSystem::initialize(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

bool InputSystem::isKeyDown(int key) const
{
    return key >= 0 && key <= GLFW_KEY_LAST && keysDown[key];
}

bool InputSystem::isMouseButtonDown(int button) const
{
    return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && mouseDown[button];
}

bool InputSystem::wasKeyPressed(int key) const
{
    for (const auto& e : keyEvents)
    {
        if (e.key == key && e.action == GLFW_PRESS)
        {
            return true;
        }
    }

    return false;
}

bool InputSystem::wasKeyReleased(int key) const
{
    for (const auto& e : keyEvents)
    {
        if (e.key == key && e.action == GLFW_RELEASE)
        {
            return true; 
        }
    }

    return false;
}

bool InputSystem::wasMousePressed(int button) const
{
    for (const auto& e : mouseEvents)
    {
        if (e.button == button && e.action == GLFW_PRESS)
        {
            return true;
        }
    }

    return false;
}

bool InputSystem::wasMouseReleased(int button) const
{
    for (const auto& e : mouseEvents)
    {
        if (e.button == button && e.action == GLFW_RELEASE)
        {
            return true;
        }
    }

    return false;
}

double InputSystem::getMouseDeltaX() const
{
    return mouseDeltaX;
}

double InputSystem::getMouseDeltaY() const
{
    return mouseDeltaY;
}

double InputSystem::getScrollY() const
{
    return scrollY;
}

const std::vector<KeyEvent>& InputSystem::getKeyEvents() const
{
    return keyEvents;
}

void InputSystem::resetMouseTracking()
{
    hasMousePosition = false;
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
}

void InputSystem::clearEvents()
{
    keyEvents.clear();
    mouseEvents.clear();

    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    scrollY = 0.0;
}

void InputSystem::keyCallback(GLFWwindow* window, int key, int, int action, int)
{
    auto* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (!input || key < 0 || key > GLFW_KEY_LAST)
        return;

    if (action == GLFW_PRESS)
    {
        input->keysDown[key] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->keysDown[key] = false;
    }
    else
    {
        return; 
    }
 

    input->keyEvents.push_back({key, action});
}

void InputSystem::mouseButtonCallback(GLFWwindow* window, int button, int action, int)
{
    auto* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (!input || button < 0 || button > GLFW_MOUSE_BUTTON_LAST)
        return;
      

    if (action == GLFW_PRESS)
    {
        input->mouseDown[button] = true;
    }
    else if (action == GLFW_RELEASE)
    {
        input->mouseDown[button] = false;
    }
    else
    {
        return;  
    }
    
    input->mouseEvents.push_back({button, action});
}

void InputSystem::cursorPositionCallback(GLFWwindow* window, double x, double y)
{
    auto* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (!input)
        return;

    if (input->hasMousePosition)
    {
        input->mouseDeltaX += x - input->lastX;
        input->mouseDeltaY += y - input->lastY;
    }

    input->lastX = x;
    input->lastY = y;
    input->hasMousePosition = true;
}

void InputSystem::scrollCallback(GLFWwindow* window, double, double yoffset)
{
    auto* input = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));

    if (input)
    {
        input->scrollY += yoffset;
    }
 
}