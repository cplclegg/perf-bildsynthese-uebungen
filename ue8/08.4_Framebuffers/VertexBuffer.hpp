#pragma once

#include <vulkan/vulkan_core.h>


class VertexBuffer {
public:
    VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, uint32_t queueFamilyIndex)
    : _physicalDevice(physicalDevice)
    ,  _device(device) {
        createBuffer(commandPool, queueFamilyIndex);
    }
    
    ~VertexBuffer() {
        cleanupBuffer();
        cleanupMemory();
    }

    VkBuffer getBuffer() {
        return _vertexBuffer;
    }

    int32_t getCount() {
        return _vertexCount;
    }

private:
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    int32_t _vertexCount;

    // create vertex data
    // std::vector<Vertex> vertices(36);
    // 1. fill vector with vertex data
    // 2. set _vertexCount
    //
    // create staging buffer
    // usage: transfer source
    //
    // allocate and bind staging buffer memory
    // 1. get memory requirements
    // 2. find memory type, that is host visible and host coherent
    // 3. allocate memory
    // 4. bind memory to buffer
    //
    // copy vertex data into staging buffer memory
    // 1. map memory
    // 2. copy data
    // 3. unmap memory
    //
    // create vertex buffer
    // (use variable _vertexBuffer)
    // usage: vertex buffer and transfer destination
    //
    // allocate and bind vertex buffer memory
    // memory properties: device local
    // 1. get memory requirements
    // 2. find memory type, that is device local
    // 3. allocate memory (use variable _vertexBufferMemory)
    // 4. bind memory to buffer
    //
    // copy data from staging buffer to vertex buffer
    // 1. allocate command buffer (use commandPool)
    // 2. begin recording command buffer
    // 3. add copy buffer command
    // 4. end recording
    // 5. get graphics queue (use queueFamilyIndex)
    // 6. submit command buffer to queue
    // 7. wait for queue to become idle
    // 8. free command buffer
    //
    // destroy staging buffer and memory
    void createBuffer(VkCommandPool commandPool, uint32_t queueFamilyIndex);

    // destroy vertex buffer
    void cleanupBuffer();
    
    // free buffer memory
    void cleanupMemory();

};
