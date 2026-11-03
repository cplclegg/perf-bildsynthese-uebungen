//
// Created by tomnir on 02.11.26.
//

#include "VulkanBuffer.hpp"
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include <vector>
#include "VulkanMemory.hpp"

VulkanBuffer::VulkanBuffer(VkDevice device, VkBufferCreateFlags createFlags, size_t size, VkBufferUsageFlags usageFlags,
                           std::vector<uint32_t> queueFamilyIndices)
    : m_createInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                   .pNext = nullptr,
                   .flags = createFlags,
                   .size = size,
                   .usage = usageFlags,
                   .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                   .queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size()),
                   .pQueueFamilyIndices = queueFamilyIndices.data()},
      m_bufferHandle{VK_NULL_HANDLE},
      m_deviceHandle{device},
      m_boundMemoryHandle{VK_NULL_HANDLE},
      m_boundMemoryOffset{},
      m_boundMemorySize{},
      m_memoryRequirements{} {
  if (vkCreateBuffer(m_deviceHandle, &m_createInfo, nullptr, &m_bufferHandle) != VK_SUCCESS) {
    throw std::runtime_error("Error constructing buffer object: buffer creation failed");
  }
  vkGetBufferMemoryRequirements(m_deviceHandle, m_bufferHandle, &m_memoryRequirements);
}

VulkanBuffer::VulkanBuffer(const VulkanBuffer& other)
    : m_createInfo{other.m_createInfo},
      m_bufferHandle{VK_NULL_HANDLE},
      m_deviceHandle{other.m_deviceHandle},
      m_boundMemoryHandle{VK_NULL_HANDLE},
      m_boundMemoryOffset{},
      m_boundMemorySize{},
      m_memoryRequirements{} {
  if (vkCreateBuffer(m_deviceHandle, &m_createInfo, nullptr, &m_bufferHandle) != VK_SUCCESS) {
    throw std::runtime_error("Error constructing buffer object: buffer creation failed");
  }
  vkGetBufferMemoryRequirements(m_deviceHandle, m_bufferHandle, &m_memoryRequirements);
}

VulkanBuffer::~VulkanBuffer() {
  vkDestroyBuffer(m_deviceHandle, m_bufferHandle, nullptr);
}
