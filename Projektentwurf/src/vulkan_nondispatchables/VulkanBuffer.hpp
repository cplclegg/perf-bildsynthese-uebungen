//
// Created by tomnir on 02.11.26.
//

#ifndef PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFER_HPP
#define PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFER_HPP
#include <vulkan/vulkan_core.h>
// idee 1
/*
class VulkanBuffer
{
private:
    VkBufferCreateInfo m_createInfo;
    VkBuffer m_bufferHandle;
    VkDevice m_deviceHandle;
    VkDeviceMemory m_boundMemoryHandle;
    std::optional<void*> m_memoryPointer;
    std::vector<uint32_t> m_queueFamilyIndices;
public:
    VulkanBuffer(VkDevice device);
    ~VulkanBuffer();
    const VkBuffer handle();
    const VkDeviceMemory memory(); // evtl ein fall fuer std::expected
    const void* memoryPtr();       // auch
    void setFlags(VkBufferCreateFlags flags);
    void setSize(size_t size);
    void setUsage(VkBufferUsageFlags flags);
    void setSharingMode(VkSharingMode mode);
    void addQueueFamilyIndices(uint32_t* indexArray, size_t count);
    VkMemoryRequirements getMemoryRequirements(); // assert m_handle != VK_NULL_HANDLE
    void createHandle() // assert size != 0 && usage != 0 && !m_queueFamilyIndex.empty()
};
// */

// idee 2 besser glaubich

class VulkanBuffer
{
private:
    VkBufferCreateInfo m_createInfo;
    VkBuffer m_bufferHandle;
    VkDevice m_deviceHandle;
    VkDeviceMemory m_boundMemoryHandle;
    VkDeviceSize m_boundMemoryOffset;
    VkDeviceSize m_boundMemorySize
    VkMemoryRequirements m_memoryRequirements;
public:
    VulkanBuffer(
        VkDevice device,
        VkBufferCreateFlags flags,
        size_t size,
        VkBufferUsageFlags usageFlags,
        std::vector<uint32_t> queueFamilyIndices);
    VulkanBuffer(const VulkanBuffer& other); // create a deep copy: copy ONLY fields createinfo, devicehandle.
                                             // Create new handle with same settings, use to populate other fields
    ~VulkanBuffer(); // constructor creates handle with provided information
    void bindMemory(VulkanMemory memory, VkDeviceSize offset);   // assert memory.checkDevice(m_deviceHandle) (idea is,
                                                                 // have memory class method that returns true only if
                                                                 // memory is valid for this buffer (same device etc))
                                                                 // Also check memory.hasSufficientSpace(size)
    const VkBuffer handle();
    const VkDeviceMemory memory(); // evtl ein fall fuer std::expected
    VkMemoryRequirements getMemoryRequirements(); // assert m_handle != VK_NULL_HANDLE
};




#endif //PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFER_HPP