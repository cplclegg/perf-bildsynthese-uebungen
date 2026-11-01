#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>


class Device {
public:
    Device(VkInstance instance,
           bool (*isDeviceSuitable)(VkPhysicalDevice),
           bool (*canQueueFamilyPresent)(VkPhysicalDevice, uint32_t)) {
        pickPhysicalDevice(instance, isDeviceSuitable, canQueueFamilyPresent);
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

    uint32_t getComputeQueueFamilyIndex() {
        return _computeQueueFamilyIndex;
    }

private:
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _logicalDevice = VK_NULL_HANDLE;
    VkCommandPool _commandPool = VK_NULL_HANDLE;

    uint32_t _graphicsQueueFamilyIndex = 0;
    uint32_t _presentQueueFamilyIndex = 0;
    uint32_t _computeQueueFamilyIndex = 0;

    // check if there are any physical devices
    // throw error if not
    //
    // call isDeviceSuitable(VkPhysicalDevice device) for each physical device
    // NEW: check for each device, if it provides graphics and presentations queues
    // (call canQueueFamilyPresent(VkPhysicalDevice device, uint32_t queueFamilyIndex)
    // to check for presentation capabilities)
    // set _presentQueueFamilyIndex and _graphicsQueueFamilyIndex
    // save the first device that fulfills all requirements to _physicalDevice
    // print the name of the device
    //
    // throw an error if no device is suitable
    void pickPhysicalDevice(VkInstance instance,
                            bool (*isDeviceSuitable)(VkPhysicalDevice),
                            bool (*canQueueFamilyPresent)(VkPhysicalDevice, uint32_t));

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
