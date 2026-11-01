
// Create a descriptor set layout for the compute pipeline
// - The compute shader needs access to two storage buffers.
// - Save the layout in the member variable _descriptorSetLayout.
#include "Simulation.hpp"
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
void Simulation::createDescriptorSetLayout() {

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
}

// Create the pipeline layout
// - Use the descriptor set layout from _descriptorSetLayout.
// - Save the layout in _pipelineLayout.
void Simulation::createPipelineLayout() {
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
}

// Create a compute pipeline
// - Load the compute shader from file shaders/testapp.comp.spv.
// - Create the shader module.
// - Create the compute pipeline using the shader module and
//   the layout from _pipelineLayout.
// - Save the pipeline in the member variable _computePipeline.
// - Destroy the shader module.
void Simulation::createPipeline() {}

// Create storage buffers used by compute shader
// - Fill std::vector<Particle> with 256 particles.
// - Each particle should have position (0, 0, 0) and a random
//   velocity in ([-0.25, 0.25], [2, 2.5], [-0.25, 0.25]).
// - Create two storage buffers and fill them with the particle
//   data.
// - Use _initBuffer and _initBufferMemory for the first buffer.
// - Use _currBuffer and _currBufferMemory for the second buffer.
void Simulation::createStorageBuffers() {}

// Create a descriptor pool for both storage buffers
// - Save the descriptor pool in _descriptorPool.
void Simulation::createDescriptorPool() {}

// Allocate a descriptor set for the descriptor set layout
// - Use _descriptorPool and _descriptorSetLayout.
// - Save the descriptor set in _descriptorSet.
void Simulation::allocateDescriptorSet() {}

// Update descriptor set with buffer data
// - Use descriptor set from _descriptorSet.
// - Write descriptor set from both storage buffers
//  (_initBuffer and _currBuffer).
void Simulation::updateDescriptorSet() {}

// Create a command pool for the compute queue
// - Use the queue family index from _computeQueueIndex.
// - Save the command pool in _commandPool.
void Simulation::createCommandPool() {}

// Allocate a command buffer
// - Save the command buffer in _commandBuffer.
void Simulation::allocateCommandBuffer() {}

// Record the command buffer
// - Set flag VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT.
// - Bind the compute pipeline from _computePipeline.
// - Bind the descriptor set from _descriptorSet with layout
//   from _pipelineLayout.
// - Send a dispatch command with group counts 1, 1, 1.
void Simulation::recordCommandBuffer() {}
