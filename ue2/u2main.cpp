//
// Created by tomnir on 11.10.26.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

// Provided by VK_VERSION_1_0
#define VK_API_VERSION_VARIANT(version) ((uint32_t)(version) >> 29U)
// Provided by VK_VERSION_1_0
#define VK_API_VERSION_MAJOR(version) (((uint32_t)(version) >> 22U) & 0x7FU)
// Provided by VK_VERSION_1_0
#define VK_API_VERSION_MINOR(version) (((uint32_t)(version) >> 12U) & 0x3FFU)
// Provided by VK_VERSION_1_0
#define VK_API_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFFU)

int main()
{
    VkApplicationInfo applicationInfo {};
    VkInstance instance;
    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        std::clog << "Error creating instance" << std::endl;
    }

    uint32_t layerPropertyCount;
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);
    std::vector<VkLayerProperties> layerProperties{layerPropertyCount};
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, layerProperties.data());

    uint32_t extensionPropertyCount;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties{extensionPropertyCount};
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionPropertyCount, extensionProperties.data());

    uint32_t deviceCount {0};
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices {deviceCount};
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::vector<VkPhysicalDeviceProperties> deviceProperties {deviceCount};
    std::vector<VkPhysicalDeviceFeatures> deviceFeatures {deviceCount};
    for (int i = 0; i<deviceCount; ++i)
    {
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties[i]);
        vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures[i]);
    }

    std::cout << "Supported extensions:" << std::endl << std::endl;
    for (auto& vk_ext_prop : extensionProperties)
    {
        std::cout << "\tName: " << vk_ext_prop.extensionName << std::endl;
        std::cout << "\tSpec version: " << vk_ext_prop.specVersion << std::endl << std::endl;
    }
    std::cout << std::endl << "Supported layers:" << std::endl << std::endl;
    for (auto& vk_layer_prop : layerProperties)
    {
        std::cout << "\tName: " << vk_layer_prop.layerName << std::endl;
        std::cout << "\tDescr.: " << vk_layer_prop.description << std::endl;
        std::cout << "\tSpec version: " << vk_layer_prop.specVersion << std::endl;
        std::cout << "\tImplementation version: " << vk_layer_prop.implementationVersion << std::endl << std::endl;
    }
    std::cout << "Available physical devices: " << std::endl << std::endl;
    int k {0};
    for (auto& vk_pdvc_props : deviceProperties)
    {
        std::cout << "\tDevice name: " << vk_pdvc_props.deviceName << std::endl;
        std::cout << "\tDevice type: ";
        switch (vk_pdvc_props.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            std::cout << "Other" << std::endl;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            std::cout << "Integrated GPU" << std::endl;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            std::cout << "Discrete GPU" << std::endl;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            std::cout << "Virtual GPU" << std::endl;
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            std::cout << "CPU" << std::endl;
            break;
        default:
            break;
        }

        std::cout << "\tFeatures supported by device: " << std::endl;
        if (deviceFeatures[k].robustBufferAccess) std::cout << "\t\tRobust buffer access\n";
        if (deviceFeatures[k].fullDrawIndexUint32) std::cout << "\t\tFull draw index uint32\n";
        if (deviceFeatures[k].imageCubeArray) std::cout << "\t\tImage cube array\n";
        if (deviceFeatures[k].independentBlend) std::cout << "\t\tIndependent blend\n";
        if (deviceFeatures[k].geometryShader) std::cout << "\t\tGeometry shader\n";
        if (deviceFeatures[k].tessellationShader) std::cout << "\t\tTessellation shader\n";
        if (deviceFeatures[k].sampleRateShading) std::cout << "\t\tSample rate shading\n";
        if (deviceFeatures[k].dualSrcBlend) std::cout << "\t\tDual src blend\n";
        if (deviceFeatures[k].logicOp) std::cout << "\t\tLogic op\n";
        if (deviceFeatures[k].multiDrawIndirect) std::cout << "\t\tMulti draw indirect\n";
        if (deviceFeatures[k].drawIndirectFirstInstance) std::cout << "\t\tdrawIndirectFirstInstance\n";


        uint32_t version = vk_pdvc_props.apiVersion;
        uint32_t variant = VK_API_VERSION_VARIANT(version);
        uint32_t major = VK_API_VERSION_MAJOR(version);
        uint32_t minor = VK_API_VERSION_MINOR(version);
        uint32_t patch = VK_API_VERSION_PATCH(version);
        std::cout << "\tSupported Vulkan version: " << variant << "." << major << "." << minor << "." << patch << std::endl;
        std::cout << "\tDriver version: " << vk_pdvc_props.driverVersion << std::endl;
        std::cout << "\tVendor ID: " << vk_pdvc_props.vendorID << std::endl;
        std::cout << "\tDevice ID: " << vk_pdvc_props.deviceID << std::endl;

        VkPhysicalDeviceMemoryProperties memProperties {};
        vkGetPhysicalDeviceMemoryProperties(devices[k], &memProperties);
        std::cout << "\tNumber of memory types: " << memProperties.memoryTypeCount << std::endl;
        for (uint32_t l = 0; l < memProperties.memoryTypeCount; ++l)
        {
            auto memType = memProperties.memoryTypes[l];
            auto index = memType.heapIndex;
            std::cout << "\t\tHeap Index: " << index << std::endl;
            std::string memPropFlags {};
            if ((memType.propertyFlags & 0x00000001) != 0x0) memPropFlags += "\t\tDevice local\n";
            if ((memType.propertyFlags & 0x00000002) != 0x0) memPropFlags += "\t\tHost visible\n";
            if ((memType.propertyFlags & 0x00000004) != 0x0) memPropFlags += "\t\tHost coherent\n";
            if ((memType.propertyFlags & 0x00000008) != 0x0) memPropFlags += "\t\tHost cached\n";
            if ((memType.propertyFlags & 0x00000010) != 0x0) memPropFlags += "\t\tLazily allocated\n";
            if ((memType.propertyFlags & 0x00000020) != 0x0) memPropFlags += "\t\tMemory protected\n";
            if ((memType.propertyFlags & 0x00000040) != 0x0) memPropFlags += "\t\tDevice coherent (AMD)\n";
            if ((memType.propertyFlags & 0x00000080) != 0x0) memPropFlags += "\t\tDevice uncached (AMD)\n";
            if ((memType.propertyFlags & 0x00000100) != 0x0) memPropFlags += "\t\tRDMA capable (Nvidia)\n";
            std::cout << memPropFlags << std::endl;
        }

        std::cout << "\tNumber of memory heaps: " << memProperties.memoryHeapCount << std::endl;
        for (uint32_t m = 0; m < memProperties.memoryHeapCount; ++m)
        {
            auto memHeap = memProperties.memoryHeaps[m];
            std::cout << "\t\tHeap index " << m << std::endl;
            std::cout << "\t\t  Heap size: " << memHeap.size << "bytes" << std::endl;
            std::string heapFlags {};
            if ((memHeap.flags & 0x00000001) != 0x0) heapFlags += "\t\t  Device local\n";
            if ((memHeap.flags & 0x00000002) != 0x0) heapFlags += "\t\t  Multi instance\n";
            if ((memHeap.flags & 0x00000008) != 0x0) heapFlags += "\t\t  Tile memory\n";
            std::cout << heapFlags << std::endl;
        }


        uint32_t queueFamPropCount {};
        vkGetPhysicalDeviceQueueFamilyProperties(devices[k], &queueFamPropCount, nullptr);
        std::vector<VkQueueFamilyProperties> thisDeviceQueueFamilyProperties {queueFamPropCount};
        vkGetPhysicalDeviceQueueFamilyProperties(devices[k], &queueFamPropCount, thisDeviceQueueFamilyProperties.data());

        std::cout << "\tQueue families: " << std::endl;

        for (uint32_t n = 0; n < queueFamPropCount; ++n)
        {
            auto queueFamProps = thisDeviceQueueFamilyProperties[n];
            std::cout << "\t\tQueue count: " << queueFamProps.queueCount << std::endl;
            std::string queueFlags {};

            if ((queueFamProps.queueFlags & 0x00000001) != 0x0) queueFlags += "\t\tGraphics\n";
            if ((queueFamProps.queueFlags & 0x00000002) != 0x0) queueFlags += "\t\tCompute\n";
            if ((queueFamProps.queueFlags & 0x00000004) != 0x0) queueFlags += "\t\tTransfer\n";
            if ((queueFamProps.queueFlags & 0x00000008) != 0x0) queueFlags += "\t\tSparse binding\n";
            if ((queueFamProps.queueFlags & 0x00000010) != 0x0) queueFlags += "\t\tProtected\n";
            if ((queueFamProps.queueFlags & 0x00000020) != 0x0) queueFlags += "\t\tVideo decode\n";
            if ((queueFamProps.queueFlags & 0x00000040) != 0x0) queueFlags += "\t\tVideo encode\n";
            if ((queueFamProps.queueFlags & 0x00000100) != 0x0) queueFlags += "\t\tOptical flow\n";
            if ((queueFamProps.queueFlags & 0x00000400) != 0x0) queueFlags += "\t\tData graph\n";
            std::cout << queueFlags << std::endl;
        }

        std::cout << "\t" << std::endl;
        ++k;
    }
    vkDestroyInstance(instance, nullptr);
}