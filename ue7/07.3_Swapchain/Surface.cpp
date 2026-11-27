//
// Created by tomnir on 23.11.26.
//
#include "Surface.hpp"
#include <stdexcept>
#include <vulkan/vulkan_core.h>
// call vkGetPhysicalDeviceSurfaceSupportKHR and return the result
bool Surface::canQueueFamilyPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
    VkBool32 result {VK_FALSE};
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, _surface, &result);
    return result;
}

// call queryFormats and queryPresentModes
// return true if both are not empty, otherwise return false
bool Surface::isAdequate(VkPhysicalDevice physicalDevice)
{
    auto formats = queryFormats(physicalDevice);
    auto presentModes = queryPresentModes(physicalDevice);
    return (!formats.empty() && !presentModes.empty());
}

// return the surface capabilities for the physical device
VkSurfaceCapabilitiesKHR Surface::queryCapabilities(VkPhysicalDevice physicalDevice)
{
    VkSurfaceCapabilitiesKHR capabilities {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to query physical device surface capabilities");
    }
    return capabilities;
}

// return the surface formats for the physical device
std::vector<VkSurfaceFormatKHR> Surface::queryFormats(VkPhysicalDevice physicalDevice)
{
    uint32_t count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &count, formats.data());
    return formats;
}

// return the present modes for the physical device
std::vector<VkPresentModeKHR> Surface::queryPresentModes(VkPhysicalDevice physicalDevice)
{
    uint32_t count {};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes (count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, _surface, &count, modes.data());
    return modes;
}