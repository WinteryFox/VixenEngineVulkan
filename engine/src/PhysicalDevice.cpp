#include "PhysicalDevice.h"

namespace vixen {
    PhysicalDevice::PhysicalDevice(const Instance &instance, const std::vector<const char *> &extensions) {
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

        device = pickDevice(instance, devices, extensions);
        if (device == VK_NULL_HANDLE)
            fatal("No suitable GPU found.");

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

    VkPhysicalDevice
    PhysicalDevice::pickDevice(const Instance &instance, const std::vector<VkPhysicalDevice> &devices,
                               const std::vector<const char *> &extensions) {
        int currentScore = 0;
        VkPhysicalDevice currentDevice = VK_NULL_HANDLE;

        for (const auto &physicalDevice : devices) {
            int score = 0;

            VkPhysicalDeviceProperties physicalDeviceProperties;
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

            if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;

            score += physicalDeviceProperties.limits.maxImageDimension2D;

            std::pair<std::optional<uint32_t>, std::optional<uint32_t>> pair = findQueueFamilies(instance,
                                                                                                 physicalDevice);
            if (!pair.first.has_value() || !pair.second.has_value())
                score = 0;

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> avExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, avExtensions.data());

            std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
            for (const auto &extension : avExtensions) {
                requiredExtensions.erase(extension.extensionName);
            }

            if (!requiredExtensions.empty())
                score = 0;

            if (score > 0 && score > currentScore) {
                currentScore = score;
                currentDevice = physicalDevice;
                graphicsFamilyIndex = pair.first.value();
                presentFamilyIndex = pair.second.value();
            }
        }

        return currentDevice;
    }

    std::pair<std::optional<uint32_t>, std::optional<uint32_t>>
    PhysicalDevice::findQueueFamilies(const Instance &instance, const VkPhysicalDevice &physicalDevice) {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;

        /// Get a list of all the queue families on this physical device
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        /// Loop over all of them and check if all the necessary queues are present
        uint32_t i = 0;
        for (const auto &properties : queueFamilyProperties) {
            if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphicsIndex = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, instance.surface, &presentSupport);
            if (presentSupport)
                presentIndex = i;

            i++;
        }

        return std::make_pair(graphicsIndex, presentIndex);
    }
}