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

#include "VertexBuffer.hpp"
#include "Vertex.hpp"
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
void VertexBuffer::createBuffer(VkCommandPool commandPool,
                                uint32_t queueFamilyIndex) {
  std::vector<Vertex> vertices = {
      // Front (+Z)
      {{-1.f, -1.f, 1.f}, {0.f, 0.f}},
      {{1.f, -1.f, 1.f}, {1.f, 0.f}},
      {{1.f, 1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, -1.f, 1.f}, {0.f, 0.f}},
      {{1.f, 1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, 1.f, 1.f}, {0.f, 1.f}},

      // Back (-Z)
      {{1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{-1.f, -1.f, -1.f}, {1.f, 0.f}},
      {{-1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{-1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{1.f, 1.f, -1.f}, {0.f, 1.f}},

      // Right (+X)
      {{1.f, -1.f, 1.f}, {0.f, 0.f}},
      {{1.f, -1.f, -1.f}, {1.f, 0.f}},
      {{1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{1.f, -1.f, 1.f}, {0.f, 0.f}},
      {{1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{1.f, 1.f, 1.f}, {0.f, 1.f}},

      // Left (-X)
      {{-1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{-1.f, -1.f, 1.f}, {1.f, 0.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, 1.f, -1.f}, {0.f, 1.f}},

      // Top (+Y)
      {{-1.f, 1.f, 1.f}, {0.f, 0.f}},
      {{1.f, 1.f, 1.f}, {1.f, 0.f}},
      {{1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{-1.f, 1.f, 1.f}, {0.f, 0.f}},
      {{1.f, 1.f, -1.f}, {1.f, 1.f}},
      {{-1.f, 1.f, -1.f}, {0.f, 1.f}},

      // Bottom (-Y)
      {{-1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{1.f, -1.f, -1.f}, {1.f, 0.f}},
      {{1.f, -1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, -1.f, -1.f}, {0.f, 0.f}},
      {{1.f, -1.f, 1.f}, {1.f, 1.f}},
      {{-1.f, -1.f, 1.f}, {0.f, 1.f}},
  };
  _vertexCount = 36;

  //======================================================================
  //==================== Memory properties for later =====================
  //======================================================================

  VkPhysicalDeviceMemoryProperties physicalDeviceMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &physicalDeviceMemProps);

  //======================================================================
  //===================== STAGING BUFFER =================================
  //======================================================================
  VkBuffer stagingBuffer{VK_NULL_HANDLE};

  VkBufferCreateInfo stagingBufferCreateInfo{};
  stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  stagingBufferCreateInfo.size = vertices.size() * sizeof(Vertex);
  stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(_device, &stagingBufferCreateInfo, nullptr,
                     &stagingBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Error creating staging buffer");
  }

  VkMemoryRequirements stagingMemoryRequirements{};
  vkGetBufferMemoryRequirements(_device, stagingBuffer,
                                &stagingMemoryRequirements);
  VkMemoryPropertyFlags stagingMemoryFlags =
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  uint32_t stagingTypeIndex{};
  for (uint32_t j = 0; j < physicalDeviceMemProps.memoryTypeCount; ++j) {
    if (stagingMemoryRequirements.memoryTypeBits & (1 << j) &&
        physicalDeviceMemProps.memoryTypes[j].propertyFlags &
            stagingMemoryFlags) {
      stagingTypeIndex = j;
      break;
    }
  }

  VkDeviceMemory stagingMemory{};

  VkMemoryAllocateInfo stagingMemoryAllocateInfo{};
  stagingMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  stagingMemoryAllocateInfo.allocationSize = vertices.size() * sizeof(Vertex);
  stagingMemoryAllocateInfo.memoryTypeIndex = stagingTypeIndex;

  if (vkAllocateMemory(_device, &stagingMemoryAllocateInfo, nullptr,
                       &stagingMemory) != VK_SUCCESS) {
    throw std::runtime_error("Error allocating memory for staging buffer");
  }

  //===================== Copy vertex data into stagin buffer ============\


  void *stagingBufferMemoryPointer{nullptr};
  if (vkMapMemory(_device, stagingMemory, 0, VK_WHOLE_SIZE, 0,
                  &stagingBufferMemoryPointer) != VK_SUCCESS) {
    std::cout << "Error mapping staging buffer memory" << std::endl;
  }

  std::memcpy(stagingBufferMemoryPointer, vertices.data(),
              _vertexCount * sizeof(Vertex));
  vkUnmapMemory(_device, stagingMemory);
  stagingBufferMemoryPointer = nullptr;

  //======================================================================
  //===================== Performant memory vertex buffer ================
  //======================================================================

  VkBufferCreateInfo vertexBufferCreateInfo{};
  vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  vertexBufferCreateInfo.size = vertices.size() * sizeof(Vertex);
  vertexBufferCreateInfo.usage =
      VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  vertexBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(_device, &vertexBufferCreateInfo, nullptr,
                     &_vertexBuffer) != VK_SUCCESS) {
    std::cout << "Error creating vertex buffer" << std::endl;
  }

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(_device, _vertexBuffer, &memoryRequirements);
  VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  uint32_t typeIndex{};
  for (uint32_t i = 0; i < physicalDeviceMemProps.memoryTypeCount; ++i) {
    if (memoryRequirements.memoryTypeBits & (1 << i) &&
        physicalDeviceMemProps.memoryTypes[i].propertyFlags &
            memoryProperties) {
      typeIndex = i;
      break;
    }
  }

  VkMemoryAllocateInfo allocateInfo{};
  allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocateInfo.allocationSize = vertices.size() * sizeof(Vertex);
  allocateInfo.memoryTypeIndex = typeIndex;

  if (vkAllocateMemory(_device, &allocateInfo, nullptr, &_vertexBufferMemory) !=
      VK_SUCCESS) {

    std::cout << "Error allocating memory for vertex buffer" << std::endl;
  }

  if (vkBindBufferMemory(_device, _vertexBuffer, _vertexBufferMemory, 0) !=
      VK_SUCCESS) {
    std::cout << "Error binding memory to vertex buffer" << std::endl;
  }
  //===========================================================================
  //======================== Copy data from stagin buffer =====================
  //===========================================================================
  vkAllocateCommandBuffers(_device, )
}

void VertexBuffer::cleanupBuffer() {
  vkDestroyBuffer(_device, _vertexBuffer, nullptr);
}

void VertexBuffer::cleanupMemory() {
  vkFreeMemory(_device, _vertexBufferMemory, nullptr);
}
