#include "Device.hpp"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

void Device::pickPhysicalDevice(VkInstance instance,
                                bool (*isDeviceSuitable)(VkPhysicalDevice),
                                bool (*canQueueFamilyPresent)(VkPhysicalDevice,
                                                              uint32_t)) {

  std::cout << "Picking physical device...\n";
  uint32_t physicalDeviceCount{0};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0)
    throw std::runtime_error("no physical devices found on system");

  std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                             physicalDevices.data());
  bool graphicsQueueFound{false};
  bool presentQueueFound{false};
  std::cout << "iterating physical devices\n";
  for (auto &pd : physicalDevices) {
    std::cout << "start of iteration\n";
    if (isDeviceSuitable(pd)) {
      std::cout << "\tsuitable device found. Checking queue properties..."
                << std::endl;
      uint32_t queueFamPropCount{};
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount, nullptr);
      std::vector<VkQueueFamilyProperties> queueFamProps(queueFamPropCount);
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount,
                                               queueFamProps.data());

      std::cout << "\t" << queueFamPropCount
                << " families found. checking flags..." << std::endl;

      for (uint32_t i = 0; i < queueFamPropCount; ++i) {
        auto qf = queueFamProps[i];
        if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound) {
          _graphicsQueueFamilyIndex = i;
          graphicsQueueFound = true;
          std::cout << "\tgraphics bit found\n";
        }
        if (canQueueFamilyPresent(pd, i) && !presentQueueFound) {
          _presentQueueFamilyIndex = i;
          presentQueueFound = true;
          std::cout << "\tpresentation queue family found\n";
        }
      }
      _physicalDevice = pd;
      if (graphicsQueueFound && presentQueueFound)
        break;
      std::cout << "End of iteration.\n";
    }
  }
  if (!graphicsQueueFound || !presentQueueFound) {
    throw std::runtime_error("no suitable physical device found");
  }
  std::cout << "Graphics queue family index: " << _graphicsQueueFamilyIndex
            << " Present queue family index: " << _presentQueueFamilyIndex
            << std::endl;
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
}
