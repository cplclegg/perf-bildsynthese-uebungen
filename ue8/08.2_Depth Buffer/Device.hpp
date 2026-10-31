#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>

class Surface;

class Device {
public:
    Device(VkInstance instance, Surface* surface) {
        pickPhysicalDevice(instance, surface);
        createLogicalDevice();
        createCommandPool();
    }

    ~Device() {
        destroyCommandPool();
        destroyLogicalDevice();
    }

    VkPhysicalDevice getPhysicalDevice() {
        return _physicalDevice;
    }

    VkDevice getLogicalDevice() {
        return _logicalDevice;
    }

    VkCommandPool getCommandPool() {
        return _commandPool;
    }

    uint32_t getPresentQueueFamilyIndex() {
        return _presentQueueFamilyIndex;
    }

    uint32_t getGraphicsQueueFamilyIndex() {
        return _graphicsQueueFamilyIndex;
    }

private:
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _logicalDevice = VK_NULL_HANDLE;
    VkCommandPool _commandPool = VK_NULL_HANDLE;

    uint32_t _graphicsQueueFamilyIndex = 0;
    uint32_t _presentQueueFamilyIndex = 0;

    // check if there are any physical devices
    // throw error if not
    //
    // check for each device:
    // - if it supports the extension VK_KHR_SWAPCHAIN_EXTENSION_NAME
    // - if it is adequate for the surface by calling surface->isAdequate(...)
    // - if it provides a graphics queue
    // - if it provides a presentation queue by calling surface->canQueueFamilyPresent(...)
    // return the first device that fulfills all requirements
    // set presentQueueFamilyIndex and graphicsQueueFamilyIndex
    // print the name of the device
    // throw an error if no device is suitable
    void pickPhysicalDevice(VkInstance instance, Surface* surface);

    // create a logical device with the following properties:
    // - extension "VK_KHR_swapchain" enabled
    // - feature samplerAnisotropy enabled
    // - one queue for each given queue family created
    void createLogicalDevice();

    // destroy the device
    void destroyLogicalDevice();

    // create a command pool for the graphics queue family
    void createCommandPool();

    // destroy the command pool
    void destroyCommandPool();
};
