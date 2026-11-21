#pragma once

#include <GLFW/glfw3.h>
#include <array>
#include <vector>

struct KeyEvent
{
    int key;
    int action;
};

struct MouseButtonEvent
{
    int button;
    int action;
};

class InputSystem
{
public:
    void initialize(GLFWwindow* window);

    bool isKeyDown(int key) const;
    bool isMouseButtonDown(int button) const;

    bool wasKeyPressed(int key) const;
    bool wasKeyReleased(int key) const;

    bool wasMousePressed(int button) const;
    bool wasMouseReleased(int button) const;

    double getMouseDeltaX() const;
    double getMouseDeltaY() const;
    double getScrollY() const;

    const std::vector<KeyEvent>& getKeyEvents() const;

    void resetMouseTracking();
    void clearEvents();

private:
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow*, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow*, double x, double y);
    static void scrollCallback(GLFWwindow*, double xoffset, double yoffset);

    std::array<bool, GLFW_KEY_LAST + 1> keysDown{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouseDown{};

    std::vector<KeyEvent> keyEvents;
    std::vector<MouseButtonEvent> mouseEvents;

    double lastX = 0.0;
    double lastY = 0.0;

    double mouseDeltaX = 0.0;
    double mouseDeltaY = 0.0;
    double scrollY = 0.0;

    bool hasMousePosition = false;
};