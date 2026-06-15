// Window implementation
#include <iostream>
#include "Platform/Window.h"

#pragma comment(lib,"glfw3.lib")

WindowCreator::WindowCreator()
{

}

void WindowCreator::createWindow(int &width, int &height, const char* title)
{
    glfwSetErrorCallback([](int error, const char* description) {
		std::cerr << "GLFW Error (" << error << "): " << description << std::endl;
		});
    if(!glfwInit()){
        throw std::runtime_error("Failed to initialize GLFW");
    }


	const bool fullScreen = !width || !height;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, fullScreen ? GLFW_FALSE : GLFW_TRUE);
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int x = 0; 
    int y = 0;
    int w = mode->width;
    int h = mode->height;

    if(fullScreen)
    {
		glfwGetMonitorWorkarea(monitor, &x, &y, &w, &h);
    }
    else
    {
		w = width;
        h = height;
    }
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if(!m_window){
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    if(fullScreen)
		glfwSetWindowPos(m_window, x, y);

    glfwGetWindowSize(m_window, &w, &h);
	width = w;
	height = h;

     glfwSetKeyCallback(m_window, [](GLFWwindow* window,
    int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
      glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
  });
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
