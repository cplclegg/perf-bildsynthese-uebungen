#pragma once

#include <vulkan/vulkan_core.h>

// check if there are any physical devices
// throw error if not
//
// call isDeviceSuitable(VkPhysicalDevice) for each physical device
// return the first device that is suitable
// print the name of the device
//
// throw an error if no device is suitable
VkPhysicalDevice pickPhysicalDevice(VkInstance instance, bool (*isDeviceSuitable)(VkPhysicalDevice));
