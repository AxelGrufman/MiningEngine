#include "InputMappingContext.h"

void InputMappingContext::bindKey(const std::string& action, int key)
{
    bindings.push_back({action, InputActionType::Button, InputSource::Key, key});
}

void InputMappingContext::bindMouseButton(const std::string& action, int button)
{
    bindings.push_back({action, InputActionType::Button, InputSource::MouseButton, button});
}

void InputMappingContext::bindAxis1DKey(const std::string& action, int key, float scale)
{
    bindings.push_back({action, InputActionType::Axis1D, InputSource::Key, key, {scale, 0.0f}});
}

void InputMappingContext::bindAxis2DKey(const std::string& action, int key, glm::vec2 direction)
{
    bindings.push_back({action, InputActionType::Axis2D, InputSource::Key, key, direction});
}

void InputMappingContext::bindMouseMotion(const std::string& action, glm::vec2 sensitivity)
{
    bindings.push_back({action, InputActionType::Axis2D,  InputSource::MouseMotion, 0, sensitivity});
}

void InputMappingContext::bindScrollY(const std::string& action, float scale)
{
    bindings.push_back({action, InputActionType::Axis1D, InputSource::ScrollY, 0, {scale, 0.0f}});
}