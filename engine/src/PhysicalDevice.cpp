#include "PhysicalDevice.h"

namespace vixen {
    PhysicalDevice::PhysicalDevice(const Instance &instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("There are no Vulkan supported GPUs available, consider updating your drivers.");

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
        Logger().trace(output);

        device = pickDevice(devices);
        if (device == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable GPU");

        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;
        driverProperties.pNext = nullptr;

        VkPhysicalDeviceProperties2 deviceProperties2;
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.properties = deviceProperties;
        deviceProperties2.pNext = &driverProperties;

        vkGetPhysicalDeviceProperties2(device, &deviceProperties2);

        Logger().info("Allocated a GPU with name " + std::string(deviceProperties.deviceName) + "(" +
                      std::to_string(VK_VERSION_MAJOR(deviceProperties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_MINOR(deviceProperties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_PATCH(deviceProperties.apiVersion)) + ") " +
                      "using driver " +
                      driverProperties.driverName + " " +
                      driverProperties.driverInfo);
    }

    VkPhysicalDevice PhysicalDevice::pickDevice(const std::vector<VkPhysicalDevice> &devices) {
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

            if (score > currentScore) {
                currentScore = score;
                currentDevice = physicalDevice;
            }
        }

        return currentDevice;
    }
}