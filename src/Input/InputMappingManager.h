#pragma once

#include <string>
#include <vector>
#include "InputMappingContext.h"

class InputMappingManager
{
public:
    void addContext(const InputMappingContext& context);
    bool setContextEnabled(const std::string& name, bool enabled);
    bool isActionDown(const std::string& actionName, const InputSystem& input) const;
    glm::vec2 getAxis2D(const std::string& actionName, const InputSystem& input) const;
private:
    std::vector<InputMappingContext> contexts;
};