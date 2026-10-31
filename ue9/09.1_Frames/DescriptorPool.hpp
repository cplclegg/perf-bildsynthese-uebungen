#pragma once

#include <vulkan/vulkan_core.h>

class DescriptorPool {
public:
    DescriptorPool(VkDevice device, uint32_t framesInFlight) 
    : _device(device) {
        createDescriptorPool(framesInFlight);
    }

    ~DescriptorPool() {
        destroyDescriptorPool();
    }

    VkDescriptorPool getDescriptorPool() {
        return _descriptorPool;
    }

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    
    // create a descritor pool
    // - the number of uniform buffers is given by framesInFlight
    // - the number of combined image samplers is also given by framesInFlight
    void createDescriptorPool(uint32_t framesInFlight);

    // destroy the descriptor pool
    void destroyDescriptorPool();
};