//
// Created by tomnir on 02.11.26.
//

#ifndef PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFERFLAGS_HPP
#define PERF_BILDSYNTHESE_UEBUNGEN_VULKANBUFFERFLAGS_HPP


class VulkanBufferFlags
{
private:
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