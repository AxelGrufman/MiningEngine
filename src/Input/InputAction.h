#pragma once

#include <string>

enum class InputActionType
{
    Button,
    Axis1D,
    Axis2D
};

struct InputAction
{
    std::string name;
    InputActionType type;
};