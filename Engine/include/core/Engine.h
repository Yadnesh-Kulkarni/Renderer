#pragma once
#include <memory>
#include "renderer/Renderer.h"
#include "platform/Window.h"

class EngineCore {
private:
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<WindowCreator> window;
public:
    EngineCore();
    ~EngineCore();
    
    void Run();
};