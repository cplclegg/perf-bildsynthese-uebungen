//
// Created by tomnir on 11.10.26.
//

typedef struct VkInstanceCreateInfo
{
    VkStructureType sType;
    const void* pNext;
    VkInstanceCreateFlags flags;
    const VkApplicationInfo* pApplicationInfo;
    uint32_t enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef struct VkApplicationInfo
{
    VkStructureType sType;
    const void* pNext;
    const char* pApplicationName;
    uint32_t applicationVersion;
    const char* pEngineName;
    uint32_t engineVersion;
    uint32_t apiVerison;
} VkApplicationInfo;

typedef struct VkLayerProperties
{
    char layerName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
    uint32_t implementationVersion;
    char description[VK_MAX_DESCRIPTION_SIZE];
} VkLayerProperties;

typedef struct VkExtensionProperties
{
    char extensionName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
} VkExtensionProperties;

typedef struct VkPhysicalDeviceProperties
{
    uint32_t apiVersion;
    uint32_t driverVersion;
    uint32_t vendorID;
    uint32_t deviceID;
    VkPhysicalDeviceType deviceType;
    char deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE];
    uint8_t pipelineCacheUUID[VK_UUID_SIZE];
    VkPhysicalDeviceLimits limits;
    VkPhysicalDeviceSparseProperties sparseProperties;
} VkPhysicalDeviceProperties;

int main()
{
    VkApplicationInfo applicationInfo {};
    VkInstance instance
    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    if (vkCreateInstance(&createInfo, nullptr, &instance != VK_SUCCESS)
    {
        std::clog << "Error creating instance" << std::endl;
    }

    uint32_t layerPropertyCount;
    vkEnumerateInstanceLayerProperties(&layerPropertyCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerPropertyCount);
    vkEnumerateInstanceProperties(&layerPropertyCount, layerProperties.data());

    uint32_t extensionPropertyCount;
    vkEnumerateInstanceExtensionProperties(nullptr, extensionPropertyCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(propertyCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &propertyCount, extensionProperties.data());

    uint32_t deviceCount {0};
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
    std::vector<VkPhysicalDeviceProperties> deviceProperties(deviceCount);
    for (int i = 0; i<deviceCount; ++i)
    {
        vkGetPhysicalDevices(devices[i], &deviceProperties[i]);
    }

    std::cout << ""

    vkDestroyInstance(instance, nullptr);
}