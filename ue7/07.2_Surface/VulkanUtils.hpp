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

    //======================= Create queue and submit buffer =====================================
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

  static void copyBufferToImage(VkBuffer sourceBuffer, VkImage destinationImage, uint32_t mipLevels, VkCommandPool commandPool,
                                VkDevice device, uint32_t width, uint32_t height, VkQueue queue) {

    // set up structures for the needed commands

    // command buffer
    VkCommandBufferAllocateInfo cmdBufferAI{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                            .pNext = nullptr,
                                            .commandPool = commandPool,
                                            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                            .commandBufferCount = 1};

    VkCommandBuffer cmdBuffer{};
    vkAllocateCommandBuffers(device, &cmdBufferAI, &cmdBuffer);

    // begin command buffer recording
    VkCommandBufferBeginInfo cmdBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                .pNext = nullptr,
                                                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
                                                .pInheritanceInfo = nullptr};

    if (vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
      throw std::runtime_error("Error beginning command buffer recording");
    }

    // RECORD COMMANDS

    // 1. transition image to transfer dst optimal

    // subresource range covering the whole image
    VkImageSubresourceRange wholeImageSrRange {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = mipLevels,
      .baseArrayLayer = 0,
      .layerCount = 1
    };

    // image memory barrier to be used for transition of image from undefined to xfer dst optimal before staging xfer
    VkImageMemoryBarrier imBarrierUndefinedToXferDstOptimal {.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                         .pNext = nullptr,
                                         .srcAccessMask = 0,
                                         .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                         .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                         .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                         .image = destinationImage,
                                         .subresourceRange = wholeImageSrRange};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         0, 0, nullptr, 0, nullptr, 1, &imBarrierUndefinedToXferDstOptimal);

    // copy base mip level from staging buffer to image memory

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

    // loop over miplevels and blit image accordingly

    uint32_t lastWidth = width;
    uint32_t lastHeight = height;

    for (uint32_t i = 1; i < mipLevels; ++i)
    {
      auto srcLevel = i - 1;
      auto dstLevel = i;

      // transition srcLevel from xfer dst optimal to xfer src optimal

      // subresource range for use in the pre/post blit image memory barriers
      VkImageSubresourceRange blitSrRange {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = srcLevel,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
      };

      // image memory barrier to be used for transition of image from xfer dst optimal to xfer src optimal in mipmap loop
      VkImageMemoryBarrier imBarrierXferDstToXferSrc {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = destinationImage,
        .subresourceRange = blitSrRange
      };

      vkCmdPipelineBarrier(cmdBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &imBarrierXferDstToXferSrc);

      // calculate dimensions of dst level
      uint32_t nextMipLevelWidth = std::max(1u, lastWidth/2);
      uint32_t nextMipLevelHeight = std::max(1u, lastHeight/2);

      // blit src into dst

      VkImageSubresourceLayers blitSrcSubResource {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = srcLevel, .baseArrayLayer = 0, .layerCount = 1};
      VkImageSubresourceLayers blitDstSubResource{
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .mipLevel = dstLevel, .baseArrayLayer = 0, .layerCount = 1};

      VkImageBlit imageBlit {
        .srcSubresource = blitSrcSubResource,
        .srcOffsets = {{0,0,0}, {static_cast<int32_t>(lastWidth), static_cast<int32_t>(lastHeight), 1}},
        .dstSubresource = blitDstSubResource,
        .dstOffsets { {0, 0, 0}, {static_cast<int32_t>(nextMipLevelWidth), static_cast<int32_t>(nextMipLevelHeight), 1} }
      };

      vkCmdBlitImage(
        cmdBuffer,
        destinationImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        destinationImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &imageBlit,
        VK_FILTER_LINEAR);

      // transition srcLevel to shader read only optimal

      // subresource range for transition of base mip level layouts
      VkImageSubresourceRange srcMipLevelSrRange {
        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .baseMipLevel = srcLevel,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    // image memory barrier to be used for transition of miplvl from xfer dst optimal to shader rdonly optimal post blit
    VkImageMemoryBarrier imageMemBarrierSrcTransToShdrRdonly{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                             .pNext = nullptr,
                                                             .srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
                                                             .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                             .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                                             .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                             .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                             .image = destinationImage,
                                                             .subresourceRange = srcMipLevelSrRange};

      vkCmdPipelineBarrier(
        cmdBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemBarrierSrcTransToShdrRdonly);

      // store dimensions of this iteration's dst image for use in next iteration
      lastWidth = nextMipLevelWidth;
      lastHeight = nextMipLevelHeight;
    }

    // transition LAST MIPLEVEL to shader readonly optimal

    VkImageSubresourceRange lastMipLevelSrRange {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = mipLevels-1,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
  };

    VkImageMemoryBarrier imageMemBarrierSrcTransToShdrRdonly{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                         .pNext = nullptr,
                                                         .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
                                                         .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
                                                         .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                         .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                                         .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                         .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                         .image = destinationImage,
                                                         .subresourceRange = lastMipLevelSrRange};

    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                         0, 0, nullptr, 0, nullptr, 1,
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
