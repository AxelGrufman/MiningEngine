#include "InputMappingContext.h"

#include "InputSystem.h"

bool InputMappingContext::isActionDown(const std::string& actionName, const InputSystem& input) const
{
    if (!enabled)
    {
        return false;
    }
    
    for (const KeyBinding& binding : keyBindings)
    {
        if (binding.actionName == actionName && input.isKeyDown(binding.key))
        {
            return true;
        }
    }
    for (const MouseButtonBinding& binding : mouseBindings)
    {
        if (binding.actionName == actionName && input.isMouseButtonDown(binding.button))
        {
            return true;
        }
    }

    return false;
}

glm::vec2 InputMappingContext::getAxis2D(const std::string& actionName, const InputSystem& input) const
{
    if (!enabled)
        return {0.0f, 0.0f};

    for (const MouseMotionBinding& binding : mouseMotionBindings)
    {
        if (binding.actionName == actionName)
        {
            return {static_cast<float>(input.getMouseDeltaX()), static_cast<float>(input.getMouseDeltaY())};
        }
    }

    return {0.0f, 0.0f};
}