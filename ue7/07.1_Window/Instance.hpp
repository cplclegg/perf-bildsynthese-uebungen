#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>

// check if layer "VK_LAYER_KHRONOS_validation" is availabile, throw error if not
// check if all required extensions are available, throw error if not
// create and return instance object, throw error if not successful
VkInstance createInstance(std::vector<const char*>& extensions);

// destroy the vulkan instance object
void destroyInstance(VkInstance instance);
