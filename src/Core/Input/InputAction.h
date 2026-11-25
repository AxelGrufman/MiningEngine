#pragma once

#include <glm/vec2.hpp>
#include <string>

enum class InputActionType
{
    Button,
    Axis1D,
    Axis2D
};

enum class InputPhase
{
    Started,  
    Performed,
    Canceled    
};

struct InputActionEvent
{
    std::string context;
    std::string action;
    InputActionType type;
    InputPhase phase;

    // Button: x = 0 or 1
    // Axis1D: x = axis value
    // Axis2D: x and y = axis values
    glm::vec2 value{0.0f};
};