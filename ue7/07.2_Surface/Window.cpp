//
// Created by tomnir on 23.11.26.
//

#include "Window.hpp"

#include <stdexcept>

// initialize GLFW
// create GLFW window (use _window)
// set window user pointer to this object
// set framebuffer size callback to static method framebufferResizeCallback
Window::Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(800, 600, "AAI2627", nullptr, nullptr);
    glfwSetWindowUserPointer(_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

// destroy GLFW window
// terminate GLFW
Window::~Window()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}

// return all instance extensions required by GLFW
std::vector<const char *> Window::getRequiredExtensions()
{
    uint32_t count {};
    const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char*> (requiredExtensions, requiredExtensions+count);
}

// return true if the window should close
bool Window::shouldClose()
{
    return glfwWindowShouldClose(_window);
}

// return value of member variable _framebufferResized
// reset _framebufferResized to false
bool Window::wasResized()
{
    bool temp = _framebufferResized;
    _framebufferResized = false;
    return temp;
}

// return width and height of the GLFW framebuffer
VkExtent2D Window::getExtent()
{
    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);
    return VkExtent2D {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

// poll GLFW events
void Window::pollEvents()
{
    glfwPollEvents();
}

// create a surface for this window
VkSurfaceKHR Window::createSurface(VkInstance instance)
{
    VkSurfaceKHR surface {};
    auto err = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
    if (err != VK_SUCCESS) throw std::runtime_error("Error creating window surface");
    return surface;
}

// get the window user pointer (Window object, "this" in the constructor)
// set of the Window object's member variable _framebufferResized to true
