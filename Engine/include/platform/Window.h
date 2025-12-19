// Window header
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class WindowCreator{
private:
    GLFWwindow* window;
public:
       WindowCreator();
       ~WindowCreator();
       void createWindow(int width, int height, const char* title);
       void destroyWindow();
       void pollEvents();
       void swapBuffers();
};