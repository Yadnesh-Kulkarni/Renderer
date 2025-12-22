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

void WindowCreator::createWindow(int width, int height, const char* title)
{
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!m_window){
        throw std::runtime_error("Failed to create window");
    }
}

void WindowCreator::destroyWindow()
{
    glfwDestroyWindow(m_window);
}

void WindowCreator::getRequiredExtensions(GERequiredExtensions *pRequiredExtensions)
{
    pRequiredExtensions->extensions = glfwGetRequiredInstanceExtensions(&pRequiredExtensions->count);
}

GEFramebufferSize WindowCreator::getFramebufferSize()
{
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);
    return { width, height };
}

bool WindowCreator::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}

void WindowCreator::pollEvents()
{
    glfwPollEvents();
}

void WindowCreator::swapBuffers()
{
    glfwSwapBuffers(m_window);
}
