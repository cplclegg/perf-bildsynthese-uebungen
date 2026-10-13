#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "HelloVulkan3.hpp"

#include "Instance.hpp"
#include "Device.hpp"

HelloVulkanApplication app;


// ----------------------------------------------------------------------------
// Helper Function
// ----------------------------------------------------------------------------
bool isDeviceSuitable(VkPhysicalDevice device) {
    return app.isDeviceSuitable(device);
}


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    // create instance object
    std::vector<const char*> extensions = app.getRequiredExtensions();
    VkInstance instance = createInstance(extensions);
    app.setInstance(instance);

    // pick physical device
    VkPhysicalDevice physicalDevice = pickPhysicalDevice(instance, &isDeviceSuitable);
    app.setPhysicalDevice(physicalDevice);

    // start render loop
    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    // destroy all objects
    destroyInstance(instance);

    return EXIT_SUCCESS;
}
