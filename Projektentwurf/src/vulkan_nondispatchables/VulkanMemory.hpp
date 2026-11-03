//
// Created by tomnir on 02.11.26.
//

#ifndef PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP
#define PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP
#include <cstddef>
#include <vulkan/vulkan_core.h>
// exact setup to be determined
// mapping strategy: one mapping with same lifetime as object at offest==0 and size==VK_WHOLE_SIZE
// pointer never exposed. Data written via write(void* src, size_t size, size_t offset)
// Data written to (mappedPointer+size). Write does bounds checking.
// destructor unmaps.
class VulkanMemory
{
private:
    VkMemoryAllocateInfo m_allocateInfo;
    unsigned char* m_hostAccessiblePtr;
    VkDevice m_deviceHandle;
    VkDeviceMemory m_memoryHandle;
public:
    void write(const void* source, const std::size_t size, const std::size_t offset);
};


#endif //PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP