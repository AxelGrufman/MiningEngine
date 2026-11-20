#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <array>

struct KeyEvent
{  
    int key;
    int action;
};

class InputSystem
{
public:
    void initialize(GLFWwindow* window);
    bool isKeyDown(int key) const;
    bool isMouseButtonDown(int button) const;
    void clearEvents();
    const std::vector<KeyEvent>& getKeyEvents() const;
    double getMouseDeltaX() const;
    double getMouseDeltaY() const;
    

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    std::vector<KeyEvent> keyEvents;
    std::array<bool, GLFW_KEY_LAST + 1> keysDown{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouseButtonsDown{};
    
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;

    bool hasMousePosition = false;
};