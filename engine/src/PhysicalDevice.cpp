#include "PhysicalDevice.h"

namespace vixen {
    PhysicalDevice::PhysicalDevice(const Instance &instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            fatal(
                    "There are no Vulkan supported GPUs available, updating your graphics drivers may fix this.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, devices.data());

        std::string output = "Found " + std::to_string(devices.size()) + " physical devices; ";
        for (const auto &physicalDevice : devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            output += std::string(properties.deviceName) + "(" +
                      std::to_string(VK_VERSION_MAJOR(properties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_MINOR(properties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_PATCH(properties.apiVersion)) + ") ";
        }
        trace(output);

        std::pair<VkPhysicalDevice, std::optional<VkQueueFamilyProperties>> pair = pickDevice(devices);
        device = pair.first;
        if (device == VK_NULL_HANDLE)
            fatal("No suitable GPU found.");
        deviceQueueFamily = pair.second.value();

        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;
        driverProperties.pNext = nullptr;

        VkPhysicalDeviceProperties2 deviceProperties2;
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.properties = deviceProperties;
        deviceProperties2.pNext = &driverProperties;

        vkGetPhysicalDeviceProperties2(device, &deviceProperties2);

        info("Allocated a GPU with name " + std::string(driverProperties.driverName) + " " +
             std::string(deviceProperties.deviceName) + " " + driverProperties.driverInfo +
             " running Vulkan version " +
             std::to_string(VK_VERSION_MAJOR(deviceProperties.apiVersion)) + "." +
             std::to_string(VK_VERSION_MINOR(deviceProperties.apiVersion)) + "." +
             std::to_string(VK_VERSION_PATCH(deviceProperties.apiVersion)));
    }

    std::pair<VkPhysicalDevice, std::optional<VkQueueFamilyProperties>>
    PhysicalDevice::pickDevice(const std::vector<VkPhysicalDevice> &devices) {
        int currentScore = 0;
        VkPhysicalDevice currentDevice = VK_NULL_HANDLE;
        std::optional<VkQueueFamilyProperties> queueProperties;

        for (const auto &physicalDevice : devices) {
            int score = 0;

            VkPhysicalDeviceProperties physicalDeviceProperties;
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

            if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;

            score += physicalDeviceProperties.limits.maxImageDimension2D;

            queueProperties = findQueueFamilies(physicalDevice);
            if (!queueProperties.has_value())
                score = 0;

            if (score > 0 && score > currentScore) {
                currentScore = score;
                currentDevice = physicalDevice;
            }
        }

        return std::make_pair(currentDevice, queueProperties);
    }

    std::optional<VkQueueFamilyProperties> PhysicalDevice::findQueueFamilies(VkPhysicalDevice physicalDevice) {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        uint32_t i = 0;
        for (const auto &properties : queueFamilyProperties) {
            if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                deviceQueueFamilyIndex = i;
                return std::optional(properties);
            }
            i++;
        }

        return std::optional<VkQueueFamilyProperties>();
    }
}