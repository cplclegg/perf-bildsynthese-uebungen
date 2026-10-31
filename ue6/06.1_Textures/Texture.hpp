#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class Texture {
public:
    Texture(VkPhysicalDevice physicalDevice, VkDevice device, VkCommandPool commandPool, VkQueue queue, const char* filename)
    : _physicalDevice(physicalDevice)
    , _device(device) 
    , _commandPool(commandPool)
    , _queue(queue) {
        createImageBuffer(filename);
        createTextureImage();
        allocateTextureImageMemory();
        copyBufferToImage();
        destroyImageBuffer();
        createTextureImageView();
        createTextureSampler();
    }

    void destroy() {
        vkDestroySampler(_device, _textureSampler, nullptr);
        vkDestroyImageView(_device, _textureImageView, nullptr);
        vkDestroyImage(_device, _textureImage, nullptr);
        vkFreeMemory(_device, _textureImageMemory, nullptr);
    }

    VkImageView getImageView() {
        return _textureImageView;
    }

    VkSampler getSampler() {
        return _textureSampler;
    }

private:
    const VkFormat IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    VkCommandPool _commandPool = VK_NULL_HANDLE;
    VkQueue _queue = VK_NULL_HANDLE;

    VkBuffer _imageBuffer = VK_NULL_HANDLE;
    VkDeviceMemory _imageBufferMemory = VK_NULL_HANDLE;
    int _texWidth = 0;
    int _texHeight = 0;

    VkImage _textureImage = VK_NULL_HANDLE;
    VkDeviceMemory _textureImageMemory = VK_NULL_HANDLE;
    VkImageView _textureImageView = VK_NULL_HANDLE;
    VkSampler _textureSampler = VK_NULL_HANDLE;

    // create the staging buffer for the texture
    // - load pixel data from file with name filename
    // - save objects in member variables _imageBuffer and _imageBufferMemory
    // - save texture width and height in _texWidth and _texHeight
    void createImageBuffer(const char* filename);

    // destroy the staging buffer for the texture and free its memory
    void destroyImageBuffer();

    // create the image object for the texture
    // - use values from _texWidth and _texHeight
    // - format should be VK_FORMAT_R8G8B8A8_SRGB
    // - tiling shoud be "optimal"
    // - layout has to be "undefined"
    // - usage is "transfer destination" and "sampled"
    // - use member variable _textureImage for the image object
    void createTextureImage();

    // allocate memory for the texture image
    // - memory should be "device local"
    // - consider memory requirements of _textureImage
    // - bind image memory to _textureImage
    // - use member variable _textureImageMemory for the memory object
    void allocateTextureImageMemory();

    // copy image data from _imageBuffer to _textureImage
    // - create a command buffer from _commandPool
    // - begin recording to the command buffer
    // - add command for transition of _textureImage's layout from "undefined" to "transfer destination optimal"
    //   (source stage mask is "top of pipe" and destination stage mask "transfer")
    // - add command for copying data from _imageBuffer to _textureImage
    // - add command for transition of _textureImage's layout from "transfer destination optimal" to "shader read-only optimal"
    //   (source stage mask is "transfer" and destination stage mask "fragment shader")
    // - end recording to the command buffer
    // - submit command buffer to _queue
    // - wait for _queue to become idle
    // - free the command buffer
    void copyBufferToImage();

    // create an image view for _textureImage
    // - save to _textureImageView
    void createTextureImageView();

    // create a sampler
    // - save to _textureSampler
    void createTextureSampler();
};

