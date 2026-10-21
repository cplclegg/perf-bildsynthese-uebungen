#pragma once

#include <vulkan/vulkan_core.h>


class ImageBuffer {
public:
    ImageBuffer(VkPhysicalDevice physicalDevice, VkDevice device)
    : _physicalDevice(physicalDevice)
    ,  _device(device) {
        createBuffer();
    }
    
    ~ImageBuffer() {
        cleanupBuffer();
        cleanupMemory();
    }

    VkBuffer getBuffer() {
        return _imageBuffer;
    }

    int getWidth() {
        return _texWidth;
    }

    int getHeight() {
        return _texHeight;
    }

private:
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;

    VkBuffer _imageBuffer;
    VkDeviceMemory _imageBufferMemory;
    int _texWidth;
    int _texHeight;

    // use `stbi_load(…)` to load image with four channels per pixel
    // set _texWidth and _texHeight to the width and height of the image
    // throw error, if loading fails
    //
    // create a buffer for the image data
    // (use variable _imageBuffer)
    // set buffer usage to `VK_BUFFER_USAGE_TRANSFER_SRC_BIT`
    //
    // allocate and bind buffer memory
    // 1. get memory requirements
    // 2. find memory type, that is host visible and host coherent
    // 3. allocate memory (use variable _imageBufferMemory)
    // 4. bind memory to buffer
    //
    // copy vertex data into buffer memory
    // 1. map memory
    // 2. copy data
    // 3. unmap memory
    //
    // remove loaded pixel data with `stbi_image_free(…)`
    void createBuffer();

    // destroy vertex buffer
    void cleanupBuffer();
    
    // free buffer memory
    void cleanupMemory();

};
