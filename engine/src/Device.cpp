#include <Device.h>

namespace vixen {
    Device::Device(VkSurfaceKHR surface, Instance *instance) {
        pickPhysicalDevice(surface, instance);
        createLogicalDevice();
    }
    
    Device::~Device() {
        vkDestroyDevice(logicalDevice, nullptr);
    }
    
    void Device::pickPhysicalDevice(VkSurfaceKHR surface, Instance *instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, nullptr);
        
        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find Vulkan supported GPU");
        }
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, devices.data());
        
        for (const auto &device : devices) {
            if (isDeviceSuitable(surface, device)) {
                physicalDevice = device;
                break;
            }
        }
        
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find a suitable GPU");
        }
    }
    
    bool Device::isDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        
        // TODO: Score each device and pick device with highest score
        
        deviceName = deviceProperties.deviceName;
        
        findQueueFamilies(surface, device);
        return deviceProperties.deviceType >= VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
               deviceProperties.apiVersion >= VK_VERSION_1_1 &&
               indices.isComplete();
    }
    
    void Device::findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device) {
        uint32_t queueFamilyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
        uint32_t i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueCount > 0 && queueFamily.queueFlags && VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
    
            auto presentSupport = static_cast<VkBool32>(false);
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            
            if (queueFamily.queueCount > 0 && presentSupport) {
                indices.presentFamily = i;
            }
            
            if (indices.isComplete()) break;
            i++;
        }
    }
    
    void Device::createLogicalDevice() {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        
        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }
        
        VkPhysicalDeviceFeatures deviceFeatures = {};
        
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;

#ifdef VIXEN_DEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif
    
        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }
    
        vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
    }
}