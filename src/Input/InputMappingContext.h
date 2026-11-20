#pragma once

#include <string>
#include <vector>
#include <glm/vec2.hpp>

class InputSystem;

struct KeyBinding
{
    int key;
    std::string actionName;
};

struct MouseButtonBinding
{
    int button;
    std::string actionName;
};

struct MouseMotionBinding
{
    std::string actionName;
};

struct InputMappingContext
{
    std::string name;
    std::vector<KeyBinding> keyBindings; 
    std::vector<MouseButtonBinding> mouseBindings;
    std::vector<MouseMotionBinding> mouseMotionBindings;
    bool isActionDown(const std::string& actionName, const InputSystem& input) const;
    glm::vec2 getAxis2D(const std::string& actionName, const InputSystem& input) const;
    bool enabled = false;
};