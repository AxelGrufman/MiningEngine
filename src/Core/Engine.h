#pragma once

#include "EngineContext.h"

enum class EngineMode
{
    Editor,
    Play
};

class Engine
{
public:
    EngineMode getMode() const { return currentMode; }
    
    void enterPlayMode();
    void stopPlayMode();
    void run();
    bool isRunning() const;
    
    EngineContext& getContext()
    {
        return context;
    }

private:
    EngineMode currentMode = EngineMode::Editor;
    EngineContext context;
};
