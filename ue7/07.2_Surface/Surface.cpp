//
// Created by tomnir on 23.11.26.
//

// call vkGetPhysicalDeviceSurfaceSupportKHR and return the result
bool canQueueFamilyPresent(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
    VkBool32 result {VK_FALSE};
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, _surface, &result);
    return result;
}

// call queryFormats and queryPresentModes
// return true if both are not empty, otherwise return false
bool isAdequate(VkPhysicalDevice physicalDevice)
{
    auto formats = queryFormats(physicalDevice);
    auto presentModes = queryPresentModes(physicalDevice);
    return (!formats.empty() && !presentModes.empty());
}

// return the surface capabilities for the physical device
VkSurfaceCapabilitiesKHR queryCapabilities(VkPhysicalDevice physicalDevice)
{
    VkSurfaceCapabilitiesKHR capabilities {};
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, _surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to query physical device surface capabilities");
    }
    return capabilities;
}

// return the surface formats for the physical device
std::vector<VkSurfaceFormatKHR> queryFormats(VkPhysicalDevice physicalDevice)
{
    uint32_t count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, _surface, &count, formats.data());
    return formats;
}

// return the present modes for the physical device
std::vector<VkPresentModeKHR> queryPresentModes(VkPhysicalDevice physicalDevice)
{
    uint32_t count {};
    vkGetPhysicalDevicePresentModesKHR(physicalDevice, _surface, &count, nullptr);
    std::vector<VkPresentModeKHR> modes (count);
    vkGetPhysicalDevicePresentModesKHR(physicalDevice, _surface, &count, modes.data());
    return modes;
}