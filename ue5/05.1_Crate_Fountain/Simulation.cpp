
// Create a descriptor set layout for the compute pipeline
// - The compute shader needs access to two storage buffers.
// - Save the layout in the member variable _descriptorSetLayout.
#include "Simulation.hpp"
#include "VulkanUtils.hpp"
#include <cstring>
#include <fstream>
#include <glm/vec3.hpp>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

void Simulation::createDescriptorSetLayout() {
  std::cout << "createDescriptorSetLayout entered\n";
  std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings{};

  VkDescriptorSetLayoutBinding initial{
      .binding = 0,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .descriptorCount = 1,
      .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
      .pImmutableSamplers = nullptr};
  descriptorSetLayoutBindings.push_back(initial);

  VkDescriptorSetLayoutBinding curr{.binding = 1,
                                    .descriptorType =
                                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                    .descriptorCount = 1,
                                    .stageFlags = VK_SHADER_STAGE_COMPUTE_BIT,
                                    .pImmutableSamplers = nullptr};
  descriptorSetLayoutBindings.push_back(curr);

  VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size()),
      .pBindings = descriptorSetLayoutBindings.data()};
  if (vkCreateDescriptorSetLayout(_device, &descriptorSetLayoutCreateInfo,
                                  nullptr,
                                  &_descriptorSetLayout) != VK_SUCCESS) {
    throw std::runtime_error("Error creating descriptor set layout");
  }
  std::cout << "createDescriptorSetLayout exit\n";
}

// Create the pipeline layout
// - Use the descriptor set layout from _descriptorSetLayout.
// - Save the layout in _pipelineLayout.
void Simulation::createPipelineLayout() {
  std::cout << "createPipelineLayout entered\n";
  VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .setLayoutCount = 1,
      .pSetLayouts = &_descriptorSetLayout,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr};
  if (vkCreatePipelineLayout(_device, &pipelineLayoutCreateInfo, nullptr,
                             &_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("Error creating pipeline layout");
  }
  std::cout << "createPipelineLayout exit\n";
}

// Create a compute pipeline
// - Load the compute shader from file shaders/testapp.comp.spv.
// - Create the shader module.
// - Create the compute pipeline using the shader module and
//   the layout from _pipelineLayout.
// - Save the pipeline in the member variable _computePipeline.
// - Destroy the shader module.
void Simulation::createPipeline() {
  std::cout << "createPipeline entered\n";
  std::vector<VkComputePipelineCreateInfo> compPlCreateInfos{};

  // read compute shader code from file
  auto shaderCodeCharVector = VulkanUtils::readFile("shaders/testapp.comp.spv");
  std::cout << "\tfile read\n";

  // create compute shader module
  VkShaderModuleCreateInfo smCreateInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .codeSize = shaderCodeCharVector.size(),
      .pCode = reinterpret_cast<const uint32_t *>(shaderCodeCharVector.data())};
  VkShaderModule computeShaderModule{VK_NULL_HANDLE};

  if (vkCreateShaderModule(_device, &smCreateInfo, nullptr,
                           &computeShaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Error creating compute shader module");
  }
  std::cout << "\tmodule created\n";

  // create shader stage
  VkPipelineShaderStageCreateInfo plShaderStageCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_COMPUTE_BIT,
      .module = computeShaderModule,
      .pName = "main",
      .pSpecializationInfo = nullptr};
  std::cout << "\tshader stage create info done\n";

  // create compute pipeline

  VkComputePipelineCreateInfo compPlCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = plShaderStageCreateInfo,
      .layout = _pipelineLayout,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = 0};
  std::cout << "\tpipeline creat info done\n";
  compPlCreateInfos.push_back(compPlCreateInfo);

  if (vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1, &compPlCreateInfo,
                               nullptr, &_computePipeline) != VK_SUCCESS) {
    throw std::runtime_error("Error creating compute pipelines");
  }
  std::cout << "\tpipeline created\n";
  vkDestroyShaderModule(_device, computeShaderModule, nullptr);
  std::cout << "createPipeline exit\n";
}

// Create storage buffers used by compute shader
// - Fill std::vector<Particle> with 256 particles.
// - Each particle should have position (0, 0, 0) and a random
//   velocity in ([-0.25, 0.25], [2, 2.5], [-0.25, 0.25]).
// - Create two storage buffers and fill them with the particle
//   data.
// - Use _initBuffer and _initBufferMemory for the first buffer.
// - Use _currBuffer and _currBufferMemory for the second buffer.
void Simulation::createStorageBuffers() {
  std::cout << "createStorageBuffers entered\n";
  constexpr size_t requiredMemory{256 * sizeof(Particle)};
  VkPhysicalDeviceMemoryProperties pdMemProps{};
  vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &pdMemProps);
  //========================================================================================================
  //====================================== Create buffer for initial data
  //========================================================================================================

  VkBufferCreateInfo initBufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = requiredMemory,
      .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &_computeQueueIndex};

  if (vkCreateBuffer(_device, &initBufferCreateInfo, nullptr, &_initBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error creating init data buffer");
  }

  VkMemoryRequirements initBufferMemReq{};
  vkGetBufferMemoryRequirements(_device, _initBuffer, &initBufferMemReq);

  VkMemoryPropertyFlags initMemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  uint32_t initMemoryTypeIndex{};
  for (uint32_t j = 0; j < pdMemProps.memoryTypeCount; ++j) {
    if (initBufferMemReq.memoryTypeBits & (1 << j) &&
        (pdMemProps.memoryTypes[j].propertyFlags & initMemoryFlags) ==
            initMemoryFlags) {
      initMemoryTypeIndex = j;
      break;
    }
  }
  VkMemoryAllocateInfo initMemoryAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = initBufferMemReq.size,
      .memoryTypeIndex = initMemoryTypeIndex};
  if (vkAllocateMemory(_device, &initMemoryAllocateInfo, nullptr,
                       &_initBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("Error allocating memory for init buffer");
  }
  if (vkBindBufferMemory(_device, _initBuffer, _initBufferMemory, 0) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error binding init buffer memory");
  }
  void *initBufferMemoryPointer{nullptr};
  if (vkMapMemory(_device, _initBufferMemory, 0, VK_WHOLE_SIZE, 0,
                  &initBufferMemoryPointer) != VK_SUCCESS) {
    throw std::runtime_error("Error mapping init buffer memory");
  }

  //======================================================================================================
  //====================================== Create buffer for current data
  //======================================================================================================
  VkBufferCreateInfo currBufferCreateInfo{
      .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .size = requiredMemory,
      .usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &_computeQueueIndex};
  if (vkCreateBuffer(_device, &currBufferCreateInfo, nullptr, &_currBuffer) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error creating current buffer");
  }

  VkMemoryRequirements currMemReqs{};
  vkGetBufferMemoryRequirements(_device, _currBuffer, &currMemReqs);

  VkMemoryPropertyFlags currMemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  uint32_t currMemoryTypeIndex{};
  for (uint32_t i = 0; i < pdMemProps.memoryTypeCount; ++i) {
    if (currMemReqs.memoryTypeBits & (1 << i) &&
        (pdMemProps.memoryTypes[i].propertyFlags & currMemoryFlags) ==
            currMemoryFlags) {
      currMemoryTypeIndex = i;
      break;
    }
  }

  VkMemoryAllocateInfo currMemoryAllocateInfo{
      .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
      .pNext = nullptr,
      .allocationSize = currMemReqs.size,
      .memoryTypeIndex = currMemoryTypeIndex};
  if (vkAllocateMemory(_device, &currMemoryAllocateInfo, nullptr,
                       &_currBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("Error allocating curr buffer memory");
  }
  if (vkBindBufferMemory(_device, _currBuffer, _currBufferMemory, 0) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error binding curr buffer memory");
  }
  void *currBufferMemoryPointer{nullptr};
  if (vkMapMemory(_device, _currBufferMemory, 0, VK_WHOLE_SIZE, 0,
                  &currBufferMemoryPointer) != VK_SUCCESS) {
    throw std::runtime_error("Error mapping curr buffer memory");
  }
  //=======================================================================================================
  //============================ Create randomized data and fill buffer
  //=======================================================================================================

  std::mt19937 generator{std::random_device{}()};

  std::uniform_real_distribution<float> horizontal{-0.25f, 0.25f};
  std::uniform_real_distribution<float> vertical{2.0f, 2.5f};

  std::vector<Particle> particles;
  particles.reserve(256);

  for (int i = 0; i < 256; ++i) {
    particles.push_back(
        Particle{glm::vec3{0.0f, 0.0f, 0.0f},
                 glm::vec3{horizontal(generator), vertical(generator),
                           horizontal(generator)}});
  }
  std::memcpy(initBufferMemoryPointer, particles.data(), requiredMemory);
  std::memcpy(currBufferMemoryPointer, particles.data(), requiredMemory);
  vkUnmapMemory(_device, _initBufferMemory);
  vkUnmapMemory(_device, _currBufferMemory);

  std::cout << "createStorageBuffers exit\n";
}
// Create a descriptor pool for both storage buffers
// - Save the descriptor pool in _descriptorPool.
void Simulation::createDescriptorPool() {
  std::cout << "createDescriptorPool entered\n";
  VkDescriptorPoolSize poolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                .descriptorCount = 2};
  VkDescriptorPoolCreateInfo dpCreateInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &poolSize};
  if (vkCreateDescriptorPool(_device, &dpCreateInfo, nullptr,
                             &_descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("Error creating descriptor pool");
  }
  std::cout << "createDescriptorPool exit\n";
}

// Allocate a descriptor set for the descriptor set layout
// - Use _descriptorPool and _descriptorSetLayout.
// - Save the descriptor set in _descriptorSet.
void Simulation::allocateDescriptorSet() {
  std::cout << "allocateDescriptorSet entered\n";
  VkDescriptorSetAllocateInfo dsAllocInfo{
      .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
      .pNext = nullptr,
      .descriptorPool = _descriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &_descriptorSetLayout};
  if (vkAllocateDescriptorSets(_device, &dsAllocInfo, &_descriptorSet) !=
      VK_SUCCESS) {
    throw std::runtime_error("Error allocating descriptor set");
  }
  std::cout << "allocateDescriptorSet exit\n";
}

// Update descriptor set with buffer data
// - Use descriptor set from _descriptorSet.
// - Write descriptor set from both storage buffers
//  (_initBuffer and _currBuffer).
void Simulation::updateDescriptorSet() {
  std::cout << "updateDescriptorSet entered\n";
  std::vector<VkWriteDescriptorSet> descriptorWrites{};

  VkDescriptorBufferInfo initBufferInfo{
      .buffer = _initBuffer, .offset = 0, .range = VK_WHOLE_SIZE};

  VkWriteDescriptorSet initWriteDescSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = _descriptorSet,
      .dstBinding = 0,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .pImageInfo = nullptr,
      .pBufferInfo = &initBufferInfo,
      .pTexelBufferView = nullptr};
  descriptorWrites.push_back(initWriteDescSet);

  VkDescriptorBufferInfo currBufferInfo{
      .buffer = _currBuffer, .offset = 0, .range = VK_WHOLE_SIZE};
  VkWriteDescriptorSet currWriteDescSet{
      .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
      .pNext = nullptr,
      .dstSet = _descriptorSet,
      .dstBinding = 1,
      .dstArrayElement = 0,
      .descriptorCount = 1,
      .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
      .pImageInfo = nullptr,
      .pBufferInfo = &currBufferInfo,
      .pTexelBufferView = nullptr};
  descriptorWrites.push_back(currWriteDescSet);

  vkUpdateDescriptorSets(_device, descriptorWrites.size(),
                         descriptorWrites.data(), 0, nullptr);
  std::cout << "updateDescriptorSet exit\n";
}

// Create a command pool for the compute queue
// - Use the queue family index from _computeQueueIndex.
// - Save the command pool in _commandPool.
void Simulation::createCommandPool() {
  std::cout << "createCommandPool entered\n";
  VkCommandPoolCreateInfo commandPoolCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .queueFamilyIndex = _computeQueueIndex,
  };
  if (vkCreateCommandPool(_device, &commandPoolCreateInfo, nullptr,
                          &_commandPool) != VK_SUCCESS)
    throw std::runtime_error("Error creating command pool");
  std::cout << "createCommandPool exit\n";
}

// Allocate a command buffer
// - Save the command buffer in _commandBuffer.
void Simulation::allocateCommandBuffer() {
  std::cout << "allocateCommandBuffer entered\n";
  VkCommandBufferAllocateInfo cmdBufferAllocateInfo{};
  cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdBufferAllocateInfo.commandPool = _commandPool;
  cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdBufferAllocateInfo.commandBufferCount = 1;

  vkAllocateCommandBuffers(_device, &cmdBufferAllocateInfo, &_commandBuffer);
  std::cout << "allocateCommandBuffer exit\n";
}

// Record the command buffer
// - Set flag VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT.
// - Bind the compute pipeline from _computePipeline.
// - Bind the descriptor set from _descriptorSet with layout
//   from _pipelineLayout.
// - Send a dispatch command with group counts 1, 1, 1.
void Simulation::recordCommandBuffer() {
  std::cout << "recordCommandBuffer entered\n";
  VkCommandBufferBeginInfo beginInfo{
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
      .pNext = nullptr,
      .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
      .pInheritanceInfo = nullptr};

  vkBeginCommandBuffer(_commandBuffer, &beginInfo);
  vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                    _computePipeline);
  vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                          _pipelineLayout, 0, 1, &_descriptorSet, 0, nullptr);
  vkCmdDispatch(_commandBuffer, 1, 1, 1);
  vkEndCommandBuffer(_commandBuffer);
  std::cout << "recordCommandBuffer exit\n";
}

void Simulation::destroy() {
  std::cout << "destroy entered\n";
  vkDeviceWaitIdle(_device);
  vkDestroyBuffer(_device, _initBuffer, nullptr);
  vkDestroyBuffer(_device, _currBuffer, nullptr);
  vkFreeMemory(_device, _initBufferMemory, nullptr);
  vkFreeMemory(_device, _currBufferMemory, nullptr);
  vkFreeCommandBuffers(_device, _commandPool, 1, &_commandBuffer);
  vkDestroyCommandPool(_device, _commandPool, nullptr);
  vkFreeDescriptorSets(_device, _descriptorPool, 1, &_descriptorSet);
  vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
  vkDestroyPipeline(_device, _computePipeline, nullptr);
  vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
  std::cout << "destroy exit\n";
}
