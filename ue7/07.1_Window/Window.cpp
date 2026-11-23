//
// Created by tomnir on 23.11.26.
//

// initialize GLFW
// create GLFW window (use _window)
// set window user pointer to this object
// set framebuffer size callback to static method framebufferResizeCallback
Window()
{
    glfwInit();
    glfwWindowHint(GLFW_CIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(800, 600, "AAI2627", nullptr, nullptr);
    glfwSetWindowUserPointer(&_window, this);
    glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

// destroy GLFW window
// terminate GLFW
~Window()
{
    glfwDestroyWindow(&_window);
    glfwTerminate();
}

// return all instance extensions required by GLFW
std::vector<const char *> getRequiredExtensions()
{
    uint32_t count {};
    const char** requiredExtensions = glfwGetRequiredInstanceExtensions(&count);
    return std::vector<const char*> (requiredExtensions, requireExtensions+count);
}

// return true if the window should close
bool shouldClose()
{
    return glfwWindowShouldClose();
}

// return value of member variable _framebufferResized
// reset _framebufferResized to false
bool wasResized()
{
    bool temp = _framebufferResized;
    _framebufferResized = false;
    return temp;
}

// return width and height of the GLFW framebuffer
VkExtent2D getExtent()
{
    int width, height;
    glfwGetWindowSize(_window, &width, &height);
    return VkExtent2D {width, height};
}

// poll GLFW events
void pollEvents()
{
    glfwPollEvents();
}

// create a surface for this window
VkSurfaceKHR createSurface(VkInstance instance)
{
    VkSurfaceKHW surface {};
    auto err = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
    if (err != VK_SUCCESS) throw std::runtime_error("Error creating window surface");
    return surface;
}

// get the window user pointer (Window object, "this" in the constructor)
// set of the Window object's member variable _framebufferResized to true
static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glfwGetWindowUserPointer(window);
    window->_framebufferResized = true;
}