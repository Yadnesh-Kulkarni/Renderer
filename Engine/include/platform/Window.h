// Window header
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "common/common.h"

class WindowCreator{
private:
    GLFWwindow* m_window;
public:
       WindowCreator();
       ~WindowCreator() = default;

       GLFWwindow* GetWindowHandle() { return m_window; };
       void createWindow(int width, int height, const char* title);
       void destroyWindow();

       void getRequiredExtensions(GERequiredExtensions *pRequiredExtensions);

       bool shouldClose();
       void pollEvents();
       void swapBuffers();
};