//
// Created by tomnir on 21.10.26.
//
#include "ImageBuffer.hpp"
#include <stdexcept>
#define STB_IMAGE_IMPLEMENTATION
#include <cstring>
#include <iostream>

#include "stb_image.h"

void ImageBuffer::createBuffer() {

  auto imageData = stbi_load("textures/crate.png", &_texWidth, &_texHeight,
                             &_numberOfTexChannels, 0);
  size_t texMemorySize = _texHeight * _texWidth * _numberOfTexChannels;
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = texMemorySize;
  bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &_imageBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error creating image buffer");
  }

  VkMemoryRequirements memoryRequirements{};
  vkGetBufferMemoryRequirements(_device, _imageBuffer, &memoryRequirements);
  VkMemoryPropertyFlags memProps = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  VkPhysicalDeviceMemoryProperties physicalDeviceMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &physicalDeviceMemProps);

  uint32_t typeIndex{};
  for (uint32_t i = 0; i < physicalDeviceMemProps.memoryTypeCount; ++i) {
    if (memoryRequirements.memoryTypeBits & (1 << i) &&
        physicalDeviceMemProps.memoryTypes[i].propertyFlags & memProps) {
      typeIndex = i;
      break;
    }
  }
  VkMemoryAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocateInfo.allocationSize = texMemorySize;
  allocateInfo.memoryTypeIndex = typeIndex;

  if (vkAllocateMemory(_device, &allocateInfo, nullptr, &_imageBufferMemory) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error allocating memory for texture buffer");
  }

  if (vkBindBufferMemory(_device, _imageBuffer, _imageBufferMemory, 0) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error binding memory to vertex buffer");
  }
  void *imageBufferMemoryPointer{nullptr};
  if (vkMapMemory(_device, _imageBufferMemory, 0, VK_WHOLE_SIZE, 0,
                  &imageBufferMemoryPointer) != VK_SUCCESS) {
    throw std::runtime_error("Error mapping texture image buffer memory");
  }
  std::memcpy(imageBufferMemoryPointer, imageData, texMemorySize);
  vkUnmapMemory(_device, _imageBufferMemory);
  imageBufferMemoryPointer = nullptr;
  stbi_image_free(imageData);
  imageData = nullptr;
}

void ImageBuffer::cleanupBuffer() {
  vkDestroyBuffer(_device, _imageBuffer, nullptr);
}

void ImageBuffer::cleanupMemory() {
  vkFreeMemory(_device, _imageBufferMemory, nullptr);
}
