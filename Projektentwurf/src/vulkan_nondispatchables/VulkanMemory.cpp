//
// Created by tomnir on 02.11.26.
//

#include "VulkanMemory.hpp"

void VulkanMemory::write(void* source, size_t size, size_t offset)
{
    if (!(offset<=m_allocateInfo.size && size <= allocateInfo.size - offset))
    {
        throw std::runtime_error("Error: memory capacity exceeded in write operation");
    }
    void* destination {m_hostAccessiblePtr + offset};
    std::memcpy(destination, source, size);
}