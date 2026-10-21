//
// Created by tomnir on 21.10.26.
//
#include "VertexBuffer.hpp"
#include "Vertex.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <bits/error_constants.h>

void VertexBuffer::createBuffer() {
  float* vertexData = loadObj("models/teapot.obj");
  size_t vertexDataSize {_vertexCount * sizeof(Vertex)};
  VkBufferCreateInfo bufferCreateInfo{};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferCreateInfo.size = vertexDataSize;
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
  allocateInfo.allocationSize = vertexDataSize;
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

  std::memcpy(vertexBufferMemoryPointer, vertexData,
              vertexDataSize);
  vkUnmapMemory(_device, _vertexBufferMemory);
  vertexBufferMemoryPointer = nullptr;
  delete[] (vertexData);
  vertexData = nullptr;
}

void VertexBuffer::cleanupBuffer() {
  vkDestroyBuffer(_device, _vertexBuffer, nullptr);
}

void VertexBuffer::cleanupMemory() {
  vkFreeMemory(_device, _vertexBufferMemory, nullptr);
}
float* VertexBuffer::loadObj(const char* location)
{
    size_t lineLength = 1024;
    size_t vCount {};
    size_t vtCount {};
    size_t vnCount {};
    size_t fCount {};
    FILE* file = fopen(location, "r");
    if (file == nullptr) throw std::errc::no_such_file_or_directory;

    ssize_t read = 0;

    // line buffer for getline
    char* line = static_cast<char*>(malloc(lineLength));

    // count number of lines
    vCount = 0;
    vtCount = 0;
    vnCount = 0;
    fCount = 0;
    while ((read = getline(&line, &lineLength, file)) != -1) {
        if (strncmp(line, "v ", 2) == 0) ++vCount;
        else if (strncmp(line, "vt ", 3) == 0) ++vtCount;
        else if (strncmp(line, "vn ", 3) == 0) ++vnCount;
        else if (strncmp(line, "f ", 2) == 0) ++fCount;
    }

    // allocate temporary buffers
    auto v  = static_cast<Vector3*>(malloc(vCount * sizeof(Vector3)));
    auto vt = static_cast<Vector2*>(malloc(vtCount * sizeof(Vector2)));
    auto vn = static_cast<Vector3*>(malloc(vnCount * sizeof(Vector3)));
    // allocate output buffer
    size_t bufferSize = fCount * sizeof(Triangle);
    auto buffer = static_cast<Triangle*>(malloc(bufferSize));
    std::vector<Triangle> vertexData {};
    // reset counters
    vCount = 0;
    vtCount = 0;
    vnCount = 0;
    fCount = 0;

    // jump to start of file
    fseek(file, 0, SEEK_SET);

    // process each line
    while ((read = getline(&line, &lineLength, file)) != -1) {
        if (strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f\n", &v[vCount][0], &v[vCount][1], &v[vCount][2]);
            ++vCount;
        }
        else if (strncmp(line, "vt ", 3) == 0) {
            sscanf(line, "vt %f %f\n", &vt[vtCount][0], &vt[vtCount][1]);
            ++vtCount;
        }
        else if (strncmp(line, "vn ", 3) == 0) {
            sscanf(line, "vn %f %f %f\n", &vn[vnCount][0], &vn[vnCount][1], &vn[vnCount][2]);
            ++vnCount;
        }
        else if (strncmp(line, "f ", 2) == 0) {
            size_t idx[3][3];
            sscanf(line,
              "f %lu/%lu/%lu %lu/%lu/%lu %lu/%lu/%lu",
              &idx[0][0], &idx[0][1], &idx[0][2],
              &idx[1][0], &idx[1][1], &idx[1][2],
              &idx[2][0], &idx[2][1], &idx[2][2]
            );

            // copy data from temporary buffers to output buffer
            for (size_t i = 0; i < 3; ++i) {

                memcpy(buffer[fCount][i].pos, v[idx[i][0]-1], sizeof(Vector3));
                memcpy(buffer[fCount][i].tex, vt[idx[i][1]-1], sizeof(Vector2));
                memcpy(buffer[fCount][i].nor, vn[idx[i][2]-1], sizeof(Vector3));


            }
            ++fCount;
        }
    }

    // free temporary buffers
    free(line);
    free(v);
    free(vt);
    free(vn);

    _vertexCount = 3 * fCount;
    return reinterpret_cast<float*>(buffer);
}