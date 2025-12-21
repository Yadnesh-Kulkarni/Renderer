#pragma once
#include "renderer/Renderer.h"
#include "platform/Window.h"

class EngineCore {
private:
    Renderer *renderer;
    WindowCreator *window;
public:
    EngineCore();
    ~EngineCore();
    
    void Run();
};