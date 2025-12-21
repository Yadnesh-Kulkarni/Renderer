// Window implementation
#include <iostream>
#include "Platform/Window.h"

#pragma comment(lib,"glfw3.lib")

WindowCreator::WindowCreator()
{
    if(!glfwInit()){
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

WindowCreator::~WindowCreator()
{
    glfwTerminate();
}

void WindowCreator::createWindow(int width, int height, const char* title)
{
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!window){
        throw std::runtime_error("Failed to create window");
    }
}

void WindowCreator::destroyWindow()
{
    glfwDestroyWindow(window);
}

void WindowCreator::getRequiredExtensions(GERequiredExtensions *pRequiredExtensions)
{
    pRequiredExtensions->extensions = glfwGetRequiredInstanceExtensions(&pRequiredExtensions->count);
}

bool WindowCreator::shouldClose()
{
    return glfwWindowShouldClose(window);
}

void WindowCreator::pollEvents()
{
    glfwPollEvents();
}

void WindowCreator::swapBuffers()
{
    glfwSwapBuffers(window);
}
