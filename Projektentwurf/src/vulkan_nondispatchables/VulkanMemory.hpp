//
// Created by tomnir on 02.11.26.
//

#ifndef PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP
#define PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP

// exact setup to be determined
// mapping strategy: one mapping with same lifetime as object at offest==0 and size==VK_WHOLE_SIZE
// pointer never exposed. Data written via write(void* src, size_t size, size_t offset)
// Data written to (mappedPointer+size). Write does bounds checking.
// destructor unmaps.
class VulkanMemory
{
private:
    VkDeviceMemoryAllocateInfo m_allocateInfo;
    void* m_hostAccessiblePtr;
    VkDevice m_deviceHandle;
    VkDeviceMemory m_memoryHandle;
public:

    void write(void* source, size_t size, size_t offset)
    {
        if (!(offset<=m_allocateInfo.size && size <= allocateInfo.size - offset))
        {
            throw std::runtime_error("Error: memory capacity exceeded in write operation");
        }
        void* destination {m_hostAccessiblePtr + offset};
        std::memcpy(destination, source, size);
    }
};


#endif //PERF_BILDSYNTHESE_UEBUNGEN_VULKANMEMORY_HPP