#include <vulkan/vulkan_core.h>

#include "HelloVulkan13.hpp"

#include "Instance.hpp"
#include "Device.hpp"
#include "VertexBuffer.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Surface.hpp"
#include "Swapchain.hpp"
#include "Texture.hpp"


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    // create window object
    Window* window = new Window();

    // create instance object
    std::vector<const char*> extensions = window->getRequiredExtensions();
    VkInstance instance = createInstance(extensions);

    // create surface object
    Surface* surface = new Surface(window, instance);

    // create logical device
    Device* device = new Device(instance, surface);

    // create swap chain
    SwapChain* swapChain = new SwapChain(surface,
                                         device->getPhysicalDevice(),
                                         device->getLogicalDevice(),
                                         device->getGraphicsQueueFamilyIndex(),
                                         device->getPresentQueueFamilyIndex());
    
    // create vertex buffer
    VertexBuffer* vertexBuffer = new VertexBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), device->getGraphicsQueueFamilyIndex());

    // create texture
    VkQueue textureQueue;
    vkGetDeviceQueue(device->getLogicalDevice(), device->getGraphicsQueueFamilyIndex(), 0, &textureQueue);
    Texture texture(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), textureQueue, "textures/crate.png");

    // create helper app
    HelloVulkanApplication* app = new HelloVulkanApplication();
    app->setWindow(window);
    app->setInstance(instance);
    app->setSurface(surface);
    app->setPresentQueueFamilyIndex(device->getPresentQueueFamilyIndex());
    app->setGraphicsQueueFamilyIndex(device->getGraphicsQueueFamilyIndex());
    app->setPhysicalDevice(device->getPhysicalDevice());
    app->setLogicalDevice(device->getLogicalDevice());
    app->setSwapChain(swapChain);
    app->setCommandPool(device->getCommandPool());
    app->setVertexBuffer(vertexBuffer->getBuffer(), vertexBuffer->getCount());
    app->setTexture(texture.getImageView(), texture.getSampler());
    app->prepareRendering();

    // start render loop
    while (!window->shouldClose()) {
        window->pollEvents();
        app->render();
    }
    vkDeviceWaitIdle(device->getLogicalDevice());
    
    // destroy all objects
    delete app;
    texture.destroy();
    delete vertexBuffer;
    delete swapChain;
    delete device;
    delete surface;
    destroyInstance(instance);
    delete window;

    return EXIT_SUCCESS;
}
