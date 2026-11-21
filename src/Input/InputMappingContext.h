#pragma once

#include "InputAction.h"

#include <string>
#include <vector>
#include <unordered_map>

enum class InputSource
{
    Key,
    MouseButton,
    MouseMotion,
    ScrollY
};

struct InputBinding
{
    std::string action;
    InputActionType type;
    InputSource source;

    int code = 0;
    glm::vec2 scale{1.0f, 0.0f};
};

struct InputMappingContext
{
    std::string name;

    bool enabled = false;
    int priority = 0;
    bool consumeInput = true;

    std::vector<InputBinding> bindings;

    void bindKey(const std::string& action, int key);
    void bindMouseButton(const std::string& action, int button);
    void bindAxis1DKey(const std::string& action, int key, float scale);
    void bindAxis2DKey(const std::string& action, int key, glm::vec2 direction);
    void bindMouseMotion(const std::string& action, glm::vec2 sensitivity = {1.0f, 1.0f});
    void bindScrollY(const std::string& action, float scale = 1.0f);
    
    struct LastState
    {
        InputActionType type;
        bool active;
    };

    std::unordered_map<std::string, LastState> activeLastFrame;
};