#pragma once

#include "Surface.hpp"

class SwapChain {
public:
    SwapChain(Surface* surface, VkPhysicalDevice physicalDevice, VkDevice device, uint32_t graphicsQueueFamilyIndex, uint32_t presentQueueFamilyIndex)
    : _surface(surface)
    , _physicalDevice(physicalDevice)
    , _device(device)
    , _graphicsQueueFamilyIndex(graphicsQueueFamilyIndex)
    , _presentQueueFamilyIndex(presentQueueFamilyIndex) {
        create();
    }

    ~SwapChain() {
        cleanup();
    }

    VkSwapchainKHR getSwapChain() {
        return _swapChain;
    }

    uint32_t getImageCount() {
        return _images.size();
    }

    VkFormat getImageFormat() {
        return _imageFormat;
    }

    VkImage getImage(size_t index) {
        return _images.at(index);
    }

    VkExtent2D getExtent() {
        return _extent;
    }

    void recreate() {
        cleanup();
        create();
    }

private:
    Surface* _surface = nullptr;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    uint32_t _graphicsQueueFamilyIndex = UINT32_MAX;
    uint32_t _presentQueueFamilyIndex = UINT32_MAX;

    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
    std::vector<VkImage> _images;
    VkFormat _imageFormat;
    VkExtent2D _extent;

    // create a new swap chain and get all images
    // - query formats, present modes and capabilites from surface
    // - choose a surface format with VK_FORMAT_B8G8R8A8_SRGB and
    //   VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, or just select the first
    //   from the list
    // - set _imageFormat to the selected surface format
    // - choose present mode mailbox if available, otherwise choose
    //   present mode FIFO
    // - choose the image extent and set member variable _extent accordingly:
    //   - if available set _extent to capabilities.currentExtent
    //   - otherwise get extent from surface, but also consider
    //     capabilities.minImageExtent and capabilities.maxImageExtent
    // - choose an image count of capabilities.minImageCount + 1
    //   but also consider capabilities.maxImageCount
    // - choose an image sharing mode
    //   - if graphics and presentation queue family indices are the same,
    //     sharing mode can be "exclusive"
    //   - otherwise the sharing mode has to be "concurrent" and the queue
    //     queue family indices have to be set in the create info
    // - create the swap chain and save it to member variable _swapChain
    // - get all swap chain images and save them to _images
    void create();

    // destroy the swap chain
    void cleanup();
};
