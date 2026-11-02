//
// Created by tomnir on 02.11.26.
//

#include "VulkanBuffer.hpp"

VulkanBuffer::VulkanBuffer(VkDevice device)
    : m_createInfo {}
    , m_bufferHandle {VK_NULL_HANDLE}
    , m_deviceHandle {device}
    , m_boundMemoryHandle {VK_NULL_HANDLE}
    , m_memoryPointer {nullptr}
    , m_queueFamilyIndices {}
{
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
}

VulkanBuffer::VulkanBuffer(
    VkDevice device,
    VkBufferCreateFlags createFlags,
    size_t size,
    VkBufferUsageFlags usageFlags,
    std::vector<uint32_t> queueFamilyIndices)
        : m_createInfo {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = createFlags,
            .size = size,
            .usage = usageFlags,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = queueFamilyIndices.size(),
            .pQueueFamilyIndices = queueFamilyIndices.data()
        }
        , m_bufferHandle {VK_NULL_HANDLE}
        , m_deviceHandle {device}
        , m_boundMemoryHandle {VK_NULL_HANDLE}
        , m_memoryPointer {nullptr}
        , m_memoryRequirements {VK_NULL_HANDLE}
{
    if (vkCreateBuffer(m_device, &m_createInfo, nullptr, &m_bufferHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("Error constructing buffer object: buffer creation failed");
    }
    vkGetMemoryRequirements(m_device, m_bufferHandle, &m_memoryRequirements);
}

VulkanBuffer::VulkanBuffer(const VulkanBuffer& other)
    : m_createInfo {other.m_createInfo}
    , m_bufferHandle {VK_NULL_HANDLE}
    , m_deviceHandle {other.m_deviceHandle}
    , m_boundMemoryHandle {other.m_boundMemoryHandle}
    , m_memoryPointer {nullptr}
    , m_memoryRequirements {VK_NULL_HANDLE}
{
    if (vkCreateBuffer(m_device, &m_createInfo, nullptr, &m_bufferHandle) != VK_SUCCESS)
    {
        throw std::runtime_error("Error constructing buffer object: buffer creation failed");
    }
    vkGetMemoryRequirements(m_device, m_bufferHandle, &m_memoryRequirements);
}

VulkanBuffer::~VulkanBuffer()
{
    vkDestroyBuffer(m_device, m_bufferHandle);
}