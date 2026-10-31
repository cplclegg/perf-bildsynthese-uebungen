#include "Device.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

void Device::pickPhysicalDevice(VkInstance instance,
                                bool (*isDeviceSuitable)(VkPhysicalDevice),
                                bool (*canQueueFamilyPresent)(VkPhysicalDevice,
                                                              uint32_t)) {

  uint32_t physicalDeviceCount{0};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0)
    throw std::runtime_error("no physical devices found on system");

  std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                             physicalDevices.data());
  bool graphicsQueueFound{false};
  bool presentQueueFound{false};
  for (auto &pd : physicalDevices) {
    if (isDeviceSuitable(pd)) {
      uint32_t queueFamPropCount{};
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount, nullptr);
      std::vector<VkQueueFamilyProperties> queueFamProps(queueFamPropCount);
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount,
                                               queueFamProps.data());

      for (uint32_t i = 0; i < queueFamPropCount; ++i) {
        auto qf = queueFamProps[i];
        if ((qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !graphicsQueueFound) {
          _graphicsQueueFamilyIndex = i;
          graphicsQueueFound = true;
        }
        if (canQueueFamilyPresent(pd, i) && !presentQueueFound) {
          _presentQueueFamilyIndex = i;
          presentQueueFound = true;
        }
      }
      _physicalDevice = pd;
      if (graphicsQueueFound && presentQueueFound)
        break;
    }
  }
  if (!graphicsQueueFound || !presentQueueFound) {
    throw std::runtime_error("no suitable physical device found");
  }
}

void Device::createLogicalDevice() {

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  VkDeviceQueueCreateInfo graphicsQueueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = _graphicsQueueFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = nullptr,
  };
  queueCreateInfos.push_back(graphicsQueueCreateInfo);

  if (_graphicsQueueFamilyIndex != _presentQueueFamilyIndex) {
    VkDeviceQueueCreateInfo presentQueueCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = _presentQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = nullptr,
    };
    queueCreateInfos.push_back(presentQueueCreateInfo);
  }

  const char *extensions = "VK_KHR_swapchain";
  VkPhysicalDeviceFeatures features{};
  features.samplerAnisotropy = VK_TRUE;
  VkDeviceCreateInfo deviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = 1,
      .ppEnabledExtensionNames = &extensions,
      .pEnabledFeatures = &features,
  };

  if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr,
                     &_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("Error creating logical device");
}

void Device::destroyLogicalDevice() {
  // vkDeviceWaitIdle(_logicalDevice);
  vkDestroyDevice(_logicalDevice, nullptr);
  _logicalDevice = VK_NULL_HANDLE;
}

void Device::createCommandPool() {
  VkCommandPoolCreateInfo commandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = _graphicsQueueFamilyIndex,
  };
  if (vkCreateCommandPool(_logicalDevice, &commandPoolCreateInfo, nullptr,
                          &_commandPool) != VK_SUCCESS)
    throw std::runtime_error("Error creating command pool");
}

void Device::destroyCommandPool() {
  vkDestroyCommandPool(_logicalDevice, _commandPool, nullptr);
  _commandPool = VK_NULL_HANDLE;
}
