#include "InputMappingManager.h"
#include "InputSystem.h"

#include <algorithm>
#include <array>
#include <map>
#include <iostream>
#include <utility>

void InputMappingManager::addContext(const InputMappingContext& context)
{
    contexts.push_back(context);
    
    std::stable_sort(contexts.begin(), contexts.end(), [](const auto& a, const auto& b)
        {
            return a.priority > b.priority;
        }
    );
}

bool InputMappingManager::setContextEnabled(const std::string& name, bool enabled)
{
    for (auto& context : contexts)
    {
        if (context.name == name)
        {
            context.enabled = enabled;
            return true;
        }
    }
    return false;
}

bool InputMappingManager::rebind(const std::string& contextName, const std::string& action, InputSource source, int oldCode, int newCode)
{
    if (source != InputSource::Key && source != InputSource::MouseButton)
    {
        return false;
    }

    for (auto& context : contexts)
    {
        if (context.name != contextName)
            continue;

        for (auto& binding : context.bindings)
        {
            if (binding.action == action && binding.source == source && binding.code == oldCode)
            {
                binding.code = newCode;
                return true;
            }
        }
    }
    return false;
}

int InputMappingManager::subscribe(const std::string& context, const std::string& action, InputPhase phase, Callback callback)
{
    int id = nextId++;

    listeners.push_back({id, context, action, phase, std::move(callback)});

    return id;
}

void InputMappingManager::unsubscribe(int id)
{
    std::erase_if( listeners, [id](const Listener& listener)
        {
            return listener.id == id;
        });
}

void InputMappingManager::notify(const InputActionEvent& event)
{
    std::vector<Callback> calls;

    for (const auto& listener : listeners)
    {
        if (listener.context == event.context && listener.action == event.action && listener.phase == event.phase)
        {
            calls.push_back(listener.callback);
        }
    }

    for (auto& call : calls)
    {
        call(event);
    }
}

void InputMappingManager::process(
    const InputSystem& input)
{
    std::array<bool, GLFW_KEY_LAST + 1> usedKeys{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> usedButtons{};

    bool usedMotion = false;
    bool usedScroll = false;

    struct Result
    {
        InputActionType type;
        glm::vec2 value{0.0f};
        bool pressed = false;
    };

    for (auto& context : contexts)
    {
        if (!context.enabled)
        {
            for (auto& [name, last] : context.activeLastFrame)
            {
                if (last.active)
                {
                    notify({context.name,name, last.type, InputPhase::Canceled, glm::vec2{0.0f, 0.0f}});
                }
                last.active = false;
            }
            continue;
        }

        std::map<std::string, Result> results;
        
        auto nextUsedKeys = usedKeys;
        auto nextUsedButtons = usedButtons;

        bool nextUsedMotion = usedMotion;
        bool nextUsedScroll = usedScroll;

        for (const auto& binding : context.bindings)
        {
            auto [it, inserted] = results.try_emplace(binding.action, Result{binding.type});

            if (!inserted && it->second.type != binding.type)
            {
                std::cerr << "Conflicting action types for '" << binding.action << "' in " << context.name << '\n';
                continue;
            }

            Result& result = it->second;

            bool down = false;
            bool pressed = false;
            bool released = false;

            glm::vec2 value{0.0f};

            switch (binding.source)
            {
            case InputSource::Key:
            {
                if (binding.code < 0 || binding.code > GLFW_KEY_LAST || usedKeys[binding.code])
                {
                    continue;
                }

                down = input.isKeyDown(binding.code);
                pressed = input.wasKeyPressed(binding.code);
                released = input.wasKeyReleased(binding.code);

                if (down)
                {
                    value = binding.scale;
                }

                if ((down || pressed || released) && context.consumeInput)
                {
                    nextUsedKeys[binding.code] = true;
                }

                break;
            }

            case InputSource::MouseButton:
            {
                if (binding.code < 0 || binding.code > GLFW_MOUSE_BUTTON_LAST || usedButtons[binding.code])
                {
                    continue;
                }

                down = input.isMouseButtonDown(binding.code);
                pressed = input.wasMousePressed(binding.code);
                released = input.wasMouseReleased(binding.code);

                if (down)
                {
                    value = binding.scale;
                }
             

                if ((down || pressed || released) && context.consumeInput)
                {
                    nextUsedButtons[binding.code] = true;
                }

                break;
            }

            case InputSource::MouseMotion:
            {
                if (usedMotion)
                    continue;

                value = {static_cast<float>(input.getMouseDeltaX()) * binding.scale.x, static_cast<float>(input.getMouseDeltaY()) * binding.scale.y};

                if ((value.x != 0.0f || value.y != 0.0f) && context.consumeInput)
                {
                    nextUsedMotion = true;
                }

                break;
            }

            case InputSource::ScrollY:
            {
                if (usedScroll)
                    continue;

                value.x = static_cast<float>(input.getScrollY()) * binding.scale.x;

                if (value.x != 0.0f && context.consumeInput)
                {
                    nextUsedScroll = true;
                }
                    
                break;
            }
            }
            
            if (result.type == InputActionType::Button)
            {
                result.pressed |= pressed;

                if (down)
                {
                    result.value.x = 1.0f;
                }
            }
            else
            {
                result.value.x += value.x;

                if (result.type == InputActionType::Axis2D)
                {
                    result.value.y += value.y;
                }
            }
        }

        // Send action events to registered listeners.
        for (auto& [actionName, result] : results)
        {
            bool active = result.value.x != 0.0f || result.value.y != 0.0f;

            auto [stateIt, inserted] = context.activeLastFrame.try_emplace(actionName, InputMappingContext::LastState{result.type, false});

            bool wasActive = stateIt->second.active;

            // Started
            if ((!wasActive && active) || (!wasActive && result.pressed))
            {
                notify({context.name, actionName, result.type, InputPhase::Started, active ? result.value : glm::vec2{1.0f, 0.0f}});
            }

            // Performed
            if (active)
            {
                notify({context.name, actionName, result.type, InputPhase::Performed, result.value});
            }
            // Canceled
            if ((wasActive && !active) || (!active && result.pressed))
            {
                notify({context.name, actionName, result.type, InputPhase::Canceled, glm::vec2{0.0f, 0.0f}});
            }

            stateIt->second = {result.type, active};
        }
        
        usedKeys = nextUsedKeys;
        usedButtons = nextUsedButtons;
        usedMotion = nextUsedMotion;
        usedScroll = nextUsedScroll;
    }
}