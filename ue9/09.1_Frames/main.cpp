#include <vulkan/vulkan_core.h>

#include "Instance.hpp"
#include "Device.hpp"
#include "VertexBuffer.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Surface.hpp"
#include "Swapchain.hpp"
#include "Depthbuffer.hpp"
#include "Framebuffers.hpp"
#include "GraphicsPipeline.hpp"
#include "Scene.hpp"
#include "Frame.hpp"
#include "DescriptorPool.hpp"


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

int main() {
    // create window object
    Window* window = new Window();

    // create instance object
    std::vector<const char*> extensions = window->getRequiredExtensions();
    VkInstance instance = createInstance(extensions);

    // create surface object
    Surface* surface = new Surface(window, instance);

    // create logical device
    Device* device = new Device(instance, surface);

    // get queues
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device->getLogicalDevice(), device->getGraphicsQueueFamilyIndex(), 0, &graphicsQueue);
    VkQueue presentQueue = VK_NULL_HANDLE;
    vkGetDeviceQueue(device->getLogicalDevice(), device->getPresentQueueFamilyIndex(), 0, &presentQueue);

    // create swap chain
    SwapChain* swapChain = new SwapChain(surface,
                                         device->getPhysicalDevice(),
                                         device->getLogicalDevice(),
                                         presentQueue,
                                         device->getGraphicsQueueFamilyIndex(),
                                         device->getPresentQueueFamilyIndex());
    
    // create depth buffer
    DepthBuffer* depthBuffer = new DepthBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), swapChain->getExtent());

    // create graphics pipeline
    GraphicsPipeline* pipeline = new GraphicsPipeline(device->getLogicalDevice(), swapChain->getImageFormat(), depthBuffer->getImageFormat());

    // create framebuffers
    Framebuffers* framebuffers = new Framebuffers(device->getLogicalDevice(), swapChain, depthBuffer, pipeline->getRenderPass());

    // create descriptor pool
    const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    DescriptorPool* descriptorPool = new DescriptorPool(device->getLogicalDevice(), MAX_FRAMES_IN_FLIGHT);

    // create vertex buffer
    VertexBuffer* vertexBuffer = new VertexBuffer(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), device->getGraphicsQueueFamilyIndex());

    // create texture
    Texture texture(device->getPhysicalDevice(), device->getLogicalDevice(), device->getCommandPool(), graphicsQueue, "textures/crate.png");

    // create frames in flight
    std::vector<Frame*> framesInFlight(MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        framesInFlight[i] = new Frame(device->getPhysicalDevice(),
            device->getLogicalDevice(),
            swapChain,
            framebuffers,
            graphicsQueue,
            device->getCommandPool(),
            descriptorPool->getDescriptorPool(),
            pipeline->getDescriptorSetLayout());
    }

    // create scene object
    Scene* scene = new Scene();
    scene->setRenderObject(pipeline, vertexBuffer->getBuffer(), vertexBuffer->getCount(), texture.getImageView(), texture.getSampler());

    // start render loop
    uint32_t currentFrame = 0;
    while (!window->shouldClose()) {
        window->pollEvents();
        bool recreate = framesInFlight[currentFrame]->render(scene);
        if (recreate || window->wasResized()) {
            vkDeviceWaitIdle(device->getLogicalDevice());
            swapChain->recreate();
            depthBuffer->recreate(swapChain->getExtent());
            framebuffers->recreate();
        }
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
    vkDeviceWaitIdle(device->getLogicalDevice());
    
    // destroy all objects
    delete scene;
    texture.destroy();
    delete vertexBuffer;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        delete framesInFlight[i];
    }
    delete descriptorPool;
    delete framebuffers;
    delete pipeline;
    delete depthBuffer;
    delete swapChain;
    delete device;
    delete surface;
    destroyInstance(instance);
    delete window;

    return EXIT_SUCCESS;
}
