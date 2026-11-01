#include "Device.hpp"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

//=============================================================================
//============================= Pick physical device ==========================
//=============================================================================

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
  bool computeQueueFound{false};
  for (auto &pd : physicalDevices) {
    if (isDeviceSuitable(pd)) {
      uint32_t queueFamPropCount{};
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount, nullptr);
      std::vector<VkQueueFamilyProperties> queueFamProps(queueFamPropCount);
      vkGetPhysicalDeviceQueueFamilyProperties(pd, &queueFamPropCount,
                                               queueFamProps.data());

      for (uint32_t i = 0; i < queueFamPropCount; ++i) {
        auto qf = queueFamProps[i];
        if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphicsQueueFound) {
          _graphicsQueueFamilyIndex = i;
          graphicsQueueFound = true;
        }
        if (canQueueFamilyPresent(pd, i) && !presentQueueFound) {
          _presentQueueFamilyIndex = i;
          presentQueueFound = true;
        }
        if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT && !computeQueueFound) {
          _computeQueueFamilyIndex = i;
          computeQueueFound = true;
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

//=============================================================================
//======================== Create/destroy logical device ======================
//=============================================================================
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

  VkDeviceQueueCreateInfo computeQueueCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = _computeQueueFamilyIndex,
      .queueCount = 1,
      .pQueuePriorities = nullptr,
  };
  queueCreateInfos.push_back(computeQueueCreateInfo);

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

  std::vector<VkExtensionProperties> pdExtensionProperties{};
  uint32_t exPropCount{};
  vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &exPropCount,
                                       nullptr);
  pdExtensionProperties.resize(exPropCount);
  vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &exPropCount,
                                       pdExtensionProperties.data());

  std::vector<const char *> extensions{
      "VK_KHR_swapchain",
  };
  bool allExtensionsFound{true};
  for (auto &e : extensions) {
    bool extensionFound{false};
    for (auto &ep : pdExtensionProperties) {
      if (strcmp(e, ep.extensionName) != 0) {
        extensionFound = true;
        break;
      }
    }
    allExtensionsFound = allExtensionsFound && extensionFound;
  }
  if (!allExtensionsFound) {
    throw std::runtime_error("Error: missing extensions");
  }
  VkPhysicalDeviceFeatures availableFeatures{};
  VkPhysicalDeviceFeatures requiredFeatures{};
  requiredFeatures.samplerAnisotropy = VK_TRUE;
  vkGetPhysicalDeviceFeatures(_physicalDevice, &availableFeatures);

  if (availableFeatures.samplerAnisotropy != VK_TRUE)
    throw std::runtime_error("Error: missing feature samplerAnisotropy");

  VkDeviceCreateInfo deviceCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
      .pQueueCreateInfos = queueCreateInfos.data(),
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = nullptr,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
      .pEnabledFeatures = &requiredFeatures,
  };

  if (vkCreateDevice(_physicalDevice, &deviceCreateInfo, nullptr,
                     &_logicalDevice) != VK_SUCCESS)
    throw std::runtime_error("Error creating logical device");
}

void Device::destroyLogicalDevice() {
  // vkDeviceWaitIdle(_logicalDevice);
  vkDestroyDevice(_logicalDevice, nullptr);
}

//=============================================================================
//======================= Create/detroy logical device ========================
//=============================================================================
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
