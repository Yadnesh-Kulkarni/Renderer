// Renderer header
#include "Platform/Window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "math/GLM/vec4.hpp"
#include "math/GLM/mat4x4.hpp"


class Renderer {
private:
    WindowCreator *windowCreator;
public:

    Renderer();
    ~Renderer();

    void Initialize();
    void GameLoop();
};