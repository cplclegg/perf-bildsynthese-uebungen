#include <vulkan/vulkan_core.h>

#include "HelloVulkan10.hpp"

#include "Instance.hpp"
#include "Device.hpp"
#include "VertexBuffer.hpp"
#include "Texture.hpp"

HelloVulkanApplication app;


// ----------------------------------------------------------------------------
// Helper Function
// ----------------------------------------------------------------------------
bool isDeviceSuitable(VkPhysicalDevice device) {
    return app.isDeviceSuitable(device);
}

bool canQueueFamilyPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex) {
    return app.canQueueFamilyPresent(physicalDevice, queueFamilyIndex);
}


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    // create instance object
    std::vector<const char*> extensions = app.getRequiredExtensions();
    VkInstance instance = createInstance(extensions);
    app.setInstance(instance);

    // create logical device
    Device* device = new Device(instance, &isDeviceSuitable, &canQueueFamilyPresent);
    app.setPresentQueueFamilyIndex(device->getPresentQueueFamilyIndex());
    app.setGraphicsQueueFamilyIndex(device->getGraphicsQueueFamilyIndex());
    app.setPhysicalDevice(device->getPhysicalDevice());
    app.setLogicalDevice(device->getLogicalDevice());
    app.setCommandPool(device->getCommandPool());

    // create vertex buffer
    VertexBuffer* vertexBuffer = new VertexBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), device->getGraphicsQueueFamilyIndex());
    app.setVertexBuffer(vertexBuffer->getBuffer(), vertexBuffer->getCount());

    // create texture
    VkQueue textureQueue;
    vkGetDeviceQueue(device->getLogicalDevice(), device->getGraphicsQueueFamilyIndex(), 0, &textureQueue);
    Texture texture(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), textureQueue, "textures/crate.png");
    app.setTexture(texture.getImageView(), texture.getSampler());

    // start render loop
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // destroy all objects
    texture.destroy();
    delete vertexBuffer;
    app.destroy();
    delete device;
    destroyInstance(instance);

    return EXIT_SUCCESS;
}
