#include "VulkanUtils.hpp"
#include <stdexcept>
void copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer,
                VkDevice device, VkCommandPool commandPool,
                uint32_t queueFamilyIndex, uint32_t size) {

  VkCommandBufferAllocateInfo cmdBufferAllocateInfo{};
  cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBufferAllocateInfo.commandPool = commandPool;
  cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufferAllocateInfo.commandBufferCount = 1;

  VkCommandBuffer commandBuffer{};

  vkAllocateCommandBuffers(device, &cmdBufferAllocateInfo, &commandBuffer);

  VkCommandBufferBeginInfo cmdBufferBeginInfo{};
  cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  // cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  if (vkBeginCommandBuffer(commandBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
    throw std::runtime_error("Error beginning command buffer");
  }
  VkBufferCopy bufferCopyRegion{};
  bufferCopyRegion.size = size;
  vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1,
                  &bufferCopyRegion);
  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("Error ending command buffer recording");
  }

  //======================= Create queue =====================================
  VkQueue transferQueue{};
  vkGetDeviceQueue(device, queueFamilyIndex, 0, &transferQueue);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;

  vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);

  vkQueueWaitIdle(transferQueue);
  vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
