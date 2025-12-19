#include <iostream>
#include "renderer/Renderer.h"

int main() {
    
    Renderer renderer;

	renderer.Initialize();
	renderer.GameLoop();

    return 0;
}