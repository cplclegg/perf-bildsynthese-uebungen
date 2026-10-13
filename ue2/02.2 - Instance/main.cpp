#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "HelloVulkan2.hpp"

#include "Instance.hpp"


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    HelloVulkanApplication app;

    // create instance object
    std::vector<const char*> extensions = app.getRequiredExtensions();
    VkInstance instance = createInstance(extensions);
    app.setInstance(instance);

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
