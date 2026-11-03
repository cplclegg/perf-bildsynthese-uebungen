//
// Created by tomnir on 03.11.26.
//

#include "Texture.hpp"
#include <vulkan/vulkan_core.h>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include "VulkanUtils.hpp"
#include "stb_image.h"
// create the staging buffer for the texture
// - load pixel data from file with name filename
// - save objects in member variables _imageBuffer and _imageBufferMemory
// - save texture width and height in _texWidth and _texHeight
void Texture::createImageBuffer(const char* filename) {
  int origChannels{};
  int requestedChannels{4};
  unsigned char* image_data = stbi_load(filename, &_texWidth, &_texHeight, &origChannels, requestedChannels);

  VkBufferCreateInfo stagingBufferCI{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                     .pNext = nullptr,
                                     .flags = 0,
                                     .size = requestedChannels * _texWidth * _texHeight * sizeof(unsigned char),
                                     .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                     .queueFamilyIndexCount = 0,
                                     .pQueueFamilyIndices = nullptr};

  if (vkCreateBuffer(_device, &stagingBufferCI, nullptr, &_imageBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Error creating image buffer");
  }

  VkPhysicalDeviceMemoryProperties pdMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &pdMemProps);

  VkMemoryRequirements bufferMemReqs{};
  vkGetBufferMemoryRequirements(_device, _imageBuffer, &bufferMemReqs);

  VkMemoryPropertyFlags desiredMemProps = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
  uint32_t memoryTypeIndex{VulkanUtils::chooseMemoryType(pdMemProps, bufferMemReqs, desiredMemProps)};

  VkMemoryAllocateInfo memAllocInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                    .pNext = nullptr,
                                    .allocationSize = bufferMemReqs.size,
                                    .memoryTypeIndex = memoryTypeIndex};

  if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &_imageBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("Error allocating image memory");
  }
  if (vkBindBufferMemory(_device, _imageBuffer, _imageBufferMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("Error binding image memory to image buffer");
  }
  void* bufferMemPtr{nullptr};
  if (vkMapMemory(_device, _imageBufferMemory, 0, VK_WHOLE_SIZE, 0, &bufferMemPtr) != VK_SUCCESS) {
    throw std::runtime_error("Error mapping image buffer memory");
  }
  std::memcpy(bufferMemPtr, image_data, stagingBufferCI.size);
  stbi_image_free(image_data);
  vkUnmapMemory(_device, _imageBufferMemory);
}

// destroy the staging buffer for the texture and free its memory
void Texture::destroyImageBuffer() {
  vkFreeMemory(_device, _imageBufferMemory, nullptr);
  vkDestroyBuffer(_device, _imageBuffer, nullptr);
}

// create the image object for the texture
// - use values from _texWidth and _texHeight
// - format should be VK_FORMAT_R8G8B8A8_SRGB
// - tiling shoud be "optimal"
// - layout has to be "undefined"
// - usage is "transfer destination" and "sampled"
// - use member variable _textureImage for the image object
void Texture::createTextureImage() {
  VkImageCreateInfo imageCI{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                            .pNext = nullptr,
                            .flags = 0,
                            .imageType = VK_IMAGE_TYPE_2D,
                            .format = VK_FORMAT_R8G8B8A8_SRGB,
                            .extent = {static_cast<uint32_t>(_texWidth), static_cast<uint32_t>(_texHeight), 1},
                            .mipLevels = 1,
                            .arrayLayers = 1,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .tiling = VK_IMAGE_TILING_OPTIMAL,
                            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                            .queueFamilyIndexCount = 0,
                            .pQueueFamilyIndices = nullptr,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};
  if (vkCreateImage(_device, &imageCI, nullptr, &_textureImage) != VK_SUCCESS) {
    throw std::runtime_error("Error creating texture image");
  }
}

// allocate memory for the texture image
// - memory should be "device local"
// - consider memory requirements of _textureImage
// - bind image memory to _textureImage
// - use member variable _textureImageMemory for the memory object
void Texture::allocateTextureImageMemory() {
  VkPhysicalDeviceMemoryProperties pdMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &pdMemProps);

  VkMemoryRequirements imageMemReqs{};
  vkGetImageMemoryRequirements(_device, _textureImage, &imageMemReqs);

  VkMemoryPropertyFlags desiredMemProps = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  uint32_t memoryTypeIndex = VulkanUtils::chooseMemoryType(pdMemProps, imageMemReqs, desiredMemProps);

  VkMemoryAllocateInfo memAllocInfo{.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                    .pNext = nullptr,
                                    .allocationSize = imageMemReqs.size,
                                    .memoryTypeIndex = memoryTypeIndex};
  if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &_textureImageMemory) != VK_SUCCESS) {
    throw std::runtime_error("Error allocating texture image memory");
  }
  if (vkBindImageMemory(_device, _textureImage, _textureImageMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("Error binding texture image memory");
  }
}

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
void Texture::copyBufferToImage() {
  VulkanUtils::copyBufferToImage(_imageBuffer, _textureImage, _commandPool, _device, _texWidth * _texHeight * 4);
}

// create an image view for _textureImage
// - save to _textureImageView
void Texture::createTextureImageView() {}

// create a sampler
// - save to _textureSampler
void Texture::createTextureSampler() {}

