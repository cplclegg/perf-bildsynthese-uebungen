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
//

#include "VertexBuffer.hpp"
#include "Vertex.hpp"
#include <cstring>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>
void VertexBuffer::createBuffer() {
  std::vector<Vertex> vertices = {
      // Front (+Z) — red
      {{-1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},
      {{1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},
      {{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},
      {{-1.f, -1.f, 1.f}, {1.f, 0.f, 0.f}},
      {{1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 0.f, 0.f}},

      // Back (-Z) — green
      {{1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
      {{-1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
      {{-1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},
      {{1.f, -1.f, -1.f}, {0.f, 1.f, 0.f}},
      {{-1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},
      {{1.f, 1.f, -1.f}, {0.f, 1.f, 0.f}},

      // Right (+X) — blue
      {{1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}},
      {{1.f, -1.f, -1.f}, {0.f, 0.f, 1.f}},
      {{1.f, 1.f, -1.f}, {0.f, 0.f, 1.f}},
      {{1.f, -1.f, 1.f}, {0.f, 0.f, 1.f}},
      {{1.f, 1.f, -1.f}, {0.f, 0.f, 1.f}},
      {{1.f, 1.f, 1.f}, {0.f, 0.f, 1.f}},

      // Left (-X) — yellow
      {{-1.f, -1.f, -1.f}, {1.f, 1.f, 0.f}},
      {{-1.f, -1.f, 1.f}, {1.f, 1.f, 0.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 1.f, 0.f}},
      {{-1.f, -1.f, -1.f}, {1.f, 1.f, 0.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 1.f, 0.f}},
      {{-1.f, 1.f, -1.f}, {1.f, 1.f, 0.f}},

      // Top (+Y) — magenta
      {{-1.f, 1.f, 1.f}, {1.f, 0.f, 1.f}},
      {{1.f, 1.f, 1.f}, {1.f, 0.f, 1.f}},
      {{1.f, 1.f, -1.f}, {1.f, 0.f, 1.f}},
      {{-1.f, 1.f, 1.f}, {1.f, 0.f, 1.f}},
      {{1.f, 1.f, -1.f}, {1.f, 0.f, 1.f}},
      {{-1.f, 1.f, -1.f}, {1.f, 0.f, 1.f}},

      // Bottom (-Y) — cyan
      {{-1.f, -1.f, -1.f}, {0.f, 1.f, 1.f}},
      {{1.f, -1.f, -1.f}, {0.f, 1.f, 1.f}},
      {{1.f, -1.f, 1.f}, {0.f, 1.f, 1.f}},
      {{-1.f, -1.f, -1.f}, {0.f, 1.f, 1.f}},
      {{1.f, -1.f, 1.f}, {0.f, 1.f, 1.f}},
      {{-1.f, -1.f, 1.f}, {0.f, 1.f, 1.f}},
  };
  _vertexCount = 36;

  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
  bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  VkPhysicalDeviceMemoryProperties physicalDeviceMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &physicalDeviceMemProps);

  if (vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &_vertexBuffer) !=
      VK_SUCCESS) {
    std::cout << "Error creating vertex buffer" << std::endl;
  }

  VkMemoryRequirements memoryRequirements;
  vkGetBufferMemoryRequirements(_device, _vertexBuffer, &memoryRequirements);
  VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

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

  void *vertexBufferMemoryPointer{nullptr};
  if (vkMapMemory(_device, _vertexBufferMemory, 0, VK_WHOLE_SIZE, 0,
                  &vertexBufferMemoryPointer) != VK_SUCCESS) {
    std::cout << "Error mapping vertex buffer memory" << std::endl;
  }

  std::memcpy(vertexBufferMemoryPointer, vertices.data(),
              _vertexCount * sizeof(Vertex));
  vkUnmapMemory(_device, _vertexBufferMemory);
  vertexBufferMemoryPointer = nullptr;
}

void VertexBuffer::cleanupBuffer() {
  vkDestroyBuffer(_device, _vertexBuffer, nullptr);
}

void VertexBuffer::cleanupMemory() {
  vkFreeMemory(_device, _vertexBufferMemory, nullptr);
}
