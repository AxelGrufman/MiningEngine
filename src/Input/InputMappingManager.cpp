#include "InputMappingManager.h"

void InputMappingManager::addContext(const InputMappingContext& context)
{
    contexts.push_back(context);
}

bool InputMappingManager::setContextEnabled(const std::string& name, bool enabled)
{
    for (InputMappingContext& context : contexts)
    {
        if (context.name == name)
        {
            context.enabled = enabled;
            return true;
        }
    }

    return false;
}

bool InputMappingManager::isActionDown(const std::string& actionName, const InputSystem& input) const
{
    for (const InputMappingContext& context : contexts)
    {
        if (context.isActionDown(actionName, input))
        {
            return true;
        }
    }

    return false;
}

glm::vec2 InputMappingManager::getAxis2D(const std::string& actionName, const InputSystem& input) const
{
    for (const InputMappingContext& context : contexts)
    {
        glm::vec2 value = context.getAxis2D(actionName, input);

        if (value.x != 0.0f || value.y != 0.0f)
        {
            return value;
        }
    }

    return {0.0f, 0.0f};
}