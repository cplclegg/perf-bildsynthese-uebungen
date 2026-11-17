#pragma once
#define VK_QUEUE_FAMILY_IGNORED (~0U)
#include <vulkan/vulkan_core.h>
#include <fstream>
#include <stdexcept>
#include <vector>
class VulkanUtils {
 private:
 public:
  //==================================================================================================================
  //=================================  Copy data from sourceBuffer to DestinationBuffer ==============================
  //==================================================================================================================
  static void copyBuffer(VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkCommandPool commandPool, VkDevice device,
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
    vkCmdCopyBuffer(commandBuffer, sourceBuffer, destinationBuffer, 1, &bufferCopyRegion);
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

  //==================================================================================================================
  //=================================  Copy data from sourceBuffer to destinationImage ===============================
  //==================================================================================================================

  static void copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, VkCommandPool commandPool,
                                VkDevice device, uint32_t width, uint32_t height, VkQueue queue) {

    // set up structures for the needed commands
    VkCommandBufferAllocateInfo cmdBufferAI{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                            .pNext = nullptr,
                                            .commandPool = commandPool,
                                            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                            .commandBufferCount = 1};

    VkCommandBuffer cmdBuffer{};
    vkAllocateCommandBuffers(device, &cmdBufferAI, &cmdBuffer);

    VkImageSubresourceRange subResourceRange{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    VkImageMemoryBarrier imageMemBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                         .pNext = nullptr,
                                         .srcAccessMask = 0,
                                         .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                         .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                         .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .image = destinationImage,
                                         .subresourceRange = subResourceRange};

    VkImageMemoryBarrier imageMemBarrierSrcTransToShdrRdonly{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                             .pNext = nullptr,
                                                             .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                             .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                             .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                             .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .image = destinationImage,
                                                             .subresourceRange = subResourceRange};
    // begin command buffer recording
    VkCommandBufferBeginInfo cmdBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                .pNext = nullptr,
                                                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                .pInheritanceInfo = nullptr};

    if (vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
      throw std::runtime_error("Error beginning command buffer recording");
    }

    // submit commands
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_DEPENDENCY_DEVICE_GROUP_BIT, 0, nullptr, 0, nullptr, 1, &imageMemBarrier);
    VkImageSubresourceLayers subResource{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1};
    VkBufferImageCopy imageCopy{.bufferOffset = 0,
                                .bufferRowLength = width,
                                .bufferImageHeight = height,
                                .imageSubresource = subResource,
                                .imageOffset = {0, 0, 0},
                                .imageExtent = {width, height, 1}};
    vkCmdCopyBufferToImage(cmdBuffer, sourceBuffer, destinationImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &imageCopy);

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         VK_DEPENDENCY_DEVICE_GROUP_BIT, 0, nullptr, 0, nullptr, 1,
                         &imageMemBarrierSrcTransToShdrRdonly);
    vkEndCommandBuffer(cmdBuffer);

    // submit command buffer
    VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                            .pNext = nullptr,
                            .waitSemaphoreCount = 0,
                            .pWaitSemaphores = nullptr,
                            .pWaitDstStageMask = nullptr,
                            .commandBufferCount = 1,
                            .pCommandBuffers = &cmdBuffer,
                            .signalSemaphoreCount = 0,
                            .pSignalSemaphores = nullptr};
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);
    vkFreeCommandBuffers(device, commandPool, 1, &cmdBuffer);
  }

  //==================================================================================================================
  //================================= Read contents of filename into a vector of chars ===============================
  //==================================================================================================================
  static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file{filename, std::ios::ate | std::ios::binary};

    if (!file.is_open()) {
      throw std::runtime_error("error reading file");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
  }
  //==================================================================================================================
  //======================== Return first memory type index matching requirements and flags ==========================
  //==================================================================================================================
  static uint32_t chooseMemoryType(VkPhysicalDeviceMemoryProperties pdMemProps, VkMemoryRequirements requirements,
                                   VkMemoryPropertyFlags flags) {
    for (uint32_t i = 0; i < pdMemProps.memoryTypeCount; ++i) {
      if (requirements.memoryTypeBits & (1 << i) && (pdMemProps.memoryTypes[i].propertyFlags & flags) == flags) {
        return i;
      }
    }
    throw std::runtime_error("No suitable memory type found");
  }
};
