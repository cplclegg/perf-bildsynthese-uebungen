//
// Created by tomnir on 14.10.26.
//
#include "Instance.hpp"
#include <stdexcept>

// check if layer "VK_LAYER_KHRONOS_validation" is availabile, throw error if not
// check if all required extensions are available, throw error if not
// create and return instance object, throw error if not successful
VkInstance createInstance(std::vector<const char*>& extensions)
{

    uint32_t layerPropCount {};
    vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr);
    std::vector<VkLayerProperties> layerProperties {layerPropCount};
    vkEnumerateInstanceLayerProperties(&layerPropCount, layerProperties.data());
    bool requiredLayerFound {false};
    std::string requiredLayer {"VK_LAYER_KHRONOS_validation"};
    for (auto& lp : layerProperties)
    {
        std::string layer {lp.layerName};
        if (layer == requiredLayer) requiredLayerFound = true;
    }
    if (!requiredLayerFound) throw std::runtime_error("Validation layer not available on system");

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkInstance instance;
    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create vulkan instance");
    }
    return instance;
}

void destroyInstance(VkInstance instance)
{
    vkDestroyInstance(instance, nullptr);
}