//
// Created by tomnir on 14.10.26.
//
#include "Device.hpp"
#include <stdexcept>
#include <vector>
// check if there are any physical devices
// throw error if not
//
// call isDeviceSuitable(VkPhysicalDevice) for each physical device
// return the first device that is suitable
// print the name of the device
//
// throw an error if no device is suitable
VkPhysicalDevice
pickPhysicalDevice(VkInstance instance,
                   bool (*isDeviceSuitable)(VkPhysicalDevice)) {
  uint32_t physicalDeviceCount{0};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount == 0)
    throw std::runtime_error("no physical devices found on system");

  std::vector<VkPhysicalDevice> physicalDevices{physicalDeviceCount};
  vkEnumeratePhysicalDevices(instance, &physicalDeviceCount,
                             physicalDevices.data());
  for (auto &pd : physicalDevices) {
    if (isDeviceSuitable(pd))
      return pd;
  }
  throw std::runtime_error("no suitable physical device found");
}
