#pragma once

#include <vulkan/vulkan_core.h>
#include "Vertex.hpp"
#include <vector>

class VertexBuffer {
public:
    VertexBuffer(VkPhysicalDevice physicalDevice, VkDevice device)
    : _physicalDevice(physicalDevice)
    ,  _device(device) {
        createBuffer();
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
    // create vertex buffer
    // (use variable _vertexBuffer)
    //
    // allocate and bind buffer memory
    // 1. get memory requirements
    // 2. find memory type, that is host visible and host coherent
    // 3. allocate memory (use variable _vertexBufferMemory)
    // 4. bind memory to buffer
    //
    // copy vertex data into buffer memory
    // 1. map memory
    // 2. copy data
    // 3. unmap memory
    void createBuffer();

    // destroy vertex buffer
    void cleanupBuffer();
    
    // free buffer memory
    void cleanupMemory();

    // load data from a .obj file
    float* loadObj(const char* location);

};
