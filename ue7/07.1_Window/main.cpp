#include <vulkan/vulkan_core.h>

#include "HelloVulkan11.hpp"

#include "Instance.hpp"
#include "Device.hpp"
#include "VertexBuffer.hpp"
#include "Texture.hpp"
#include "Window.hpp"

HelloVulkanApplication* app = nullptr;


// ----------------------------------------------------------------------------
// Helper Function
// ----------------------------------------------------------------------------
bool isDeviceSuitable(VkPhysicalDevice device) {
    return app->isDeviceSuitable(device);
}

bool canQueueFamilyPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
    return app->canQueueFamilyPresent(physicalDevice, queueFamilyIndex);
}


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    // create window object
    Window* window = new Window();

    // create instance object
    std::vector<const char*> extensions = window->getRequiredExtensions();
    VkInstance instance = createInstance(extensions);

    // create helper app
    app = new HelloVulkanApplication();
    app->setWindow(window);
    app->setInstance(instance);


    // create logical device
    Device* device = new Device(instance, &isDeviceSuitable, &canQueueFamilyPresent);
    app->setPresentQueueFamilyIndex(device->getPresentQueueFamilyIndex());
    app->setGraphicsQueueFamilyIndex(device->getGraphicsQueueFamilyIndex());
    app->setPhysicalDevice(device->getPhysicalDevice());
    app->setLogicalDevice(device->getLogicalDevice());
    app->setCommandPool(device->getCommandPool());

    // create vertex buffer
    VertexBuffer* vertexBuffer = new VertexBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), device->getGraphicsQueueFamilyIndex());
    app->setVertexBuffer(vertexBuffer->getBuffer(), vertexBuffer->getCount());

    // create texture
    VkQueue textureQueue;
    vkGetDeviceQueue(device->getLogicalDevice(), device->getGraphicsQueueFamilyIndex(), 0, &textureQueue);
    Texture texture(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), textureQueue, "textures/crate.png");
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
    delete device;
    destroyInstance(instance);
    delete window;

    return EXIT_SUCCESS;
}
