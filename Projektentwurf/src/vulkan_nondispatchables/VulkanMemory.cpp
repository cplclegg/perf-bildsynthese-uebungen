//
// Created by tomnir on 02.11.26.
//

#include "VulkanMemory.hpp"

#include <cstring>
#include <stdexcept>

void VulkanMemory::write(const void* source, const size_t size, const size_t offset)
{
    if (!(offset<=m_allocateInfo.allocationSize && size <= m_allocateInfo.allocationSize - offset))
    {
        throw std::runtime_error("Error: memory capacity exceeded in write operation");
    }
    void* destination {m_hostAccessiblePtr + offset};
    std::memcpy(destination, source, size);
}
