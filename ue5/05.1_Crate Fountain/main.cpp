#include <vulkan/vulkan_core.h>

#include "HelloVulkan9.hpp"

#include "Instance.hpp"
#include "Device.hpp"
#include "VertexBuffer.hpp"
#include "ImageBuffer.hpp"
#include "Simulation.hpp"

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
    app.setComputeQueueFamilyIndex(device->getComputeQueueFamilyIndex());
    app.setPhysicalDevice(device->getPhysicalDevice());
    app.setLogicalDevice(device->getLogicalDevice());
    app.setCommandPool(device->getCommandPool());

    // create simulation
    Simulation sim(device->getPhysicalDevice(), device->getLogicalDevice(), device->getComputeQueueFamilyIndex());
    VkCommandBuffer commandBuffer = sim.getCommandBuffer();
    VkDescriptorBufferInfo bufferInfo = sim.getBufferInfo();
    app.setComputeBuffer(commandBuffer);
    app.setComputeBufferInfo(bufferInfo);

    // create vertex buffer
    VertexBuffer* vertexBuffer = new VertexBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), device->getGraphicsQueueFamilyIndex());
    app.setVertexBuffer(vertexBuffer->getBuffer(), vertexBuffer->getCount());

    // create image buffer
    ImageBuffer* imageBuffer = new ImageBuffer(device->getPhysicalDevice(), device->getLogicalDevice());
    app.setTexture(imageBuffer->getBuffer(), imageBuffer->getWidth(), imageBuffer->getHeight());
    delete imageBuffer;

    // start render loop
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // destroy all objects
    delete vertexBuffer;
    sim.destroy();
    app.destroy();
    delete device;
    destroyInstance(instance);

    return EXIT_SUCCESS;
}
