//
// Created by tomnir on 02.11.26.
//

#ifndef PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFERFLAGS_HPP
#define PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFERFLAGS_HPP


class VulkanBufferFlags
{
private:
    std::map<string, VkBufferUsageFlags> m_usageFlagMap {
        {"XFER_SRC", VK_BUFFER_USAGE_TRANSFER_SRC_BIT},
        {"XFER_DST", VK_BUFFER_USAGE_TRANSFER_DST_BIT},
        {"UNIFORM_TEXEL", VK_BUFFER_USAGE_TEXEL_BUFFER_BIT},
        {"STORAGE_TEXEL", VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},
        {"UNIFORM", VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT},
        {"STORAGE", VK_BUFFER_USAGE_STORAGE_BUFFER_BIT},
        {"INDEX", VK_BUFFER_USAGE_INDEX_BUFFER_BIT},
        {"VERTEX", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT},
        {"INDIRECT", VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT}
    }
public:
    static VkBufferUsageFlags usageFlagsVertexPerformant()
    {
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    static VkBufferUsageFlags usageFlagsStagingBuffer()
    {
        return VK_BUFFER_TRANSFER_SRC_BIT;
    }
    static VkBufferUsageFlags usageFlagsStoragePerformant()
    {
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }
    static VkBufferUsageFlags usageFlagsComputePipelineWrittenVertices()
    {
        return VK_BUFFER_USAGE_TRANSFER_DST_BIT
        | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        | VK_BUFFER_UASGE_STORAGE_BUFFER_BIT;
    }
    static VkBufferUsageFlags usageFlagsUniform()
    {
        return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    static VkBufferCreateFlags createFlagsNone()
    {
        return 0;
    }
};



#endif //PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFERFLAGS_HPP