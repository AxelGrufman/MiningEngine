#pragma once

#include "InputMappingContext.h"

#include <functional>
#include <vector>

class InputSystem;

class InputMappingManager
{
public:
    using Callback = std::function<void(const InputActionEvent&)>;

    void addContext(const InputMappingContext& context);
    bool setContextEnabled(const std::string& name, bool enabled);
    bool rebind(const std::string& context, const std::string& action, InputSource source, int oldCode, int newCode);
    int subscribe(const std::string& context, const std::string& action, InputPhase phase, Callback callback);
    void unsubscribe(int subscriptionId);
    void process(const InputSystem& input);

private:
    struct Listener
    {
        int id;
        std::string context;
        std::string action;
        InputPhase phase;
        Callback callback;
    };

    void notify(const InputActionEvent& event);

    std::vector<InputMappingContext> contexts;
    std::vector<Listener> listeners;

    int nextId = 1;
};