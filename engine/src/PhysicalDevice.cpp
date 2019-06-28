#include "PhysicalDevice.h"

namespace vixen {
    PhysicalDevice::PhysicalDevice(const Instance &instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, nullptr);

        if (deviceCount == 0)
            throw std::runtime_error("There are no Vulkan supported GPUs available, consider updating your drivers.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance.instance, &deviceCount, devices.data());

        device = pickDevice(devices);
        if (device == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable GPU");

        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        Logger().info("Allocated a GPU with name " + std::string(deviceProperties.deviceName));
    }

    PhysicalDevice::PhysicalDevice(VkPhysicalDevice device) {
        if (device == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable GPU");

        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        Logger().info("Allocated a GPU with name " + std::string(deviceProperties.deviceName));
    }

    VkPhysicalDevice PhysicalDevice::pickDevice(const std::vector<VkPhysicalDevice> &devices) {
        int currentScore = 0;
        VkPhysicalDevice currentDevice = VK_NULL_HANDLE;

        for (const auto &physicalDevice : devices) {
            int score = 0;

            VkPhysicalDeviceProperties deviceProperties;
            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;

            score += deviceProperties.limits.maxImageDimension2D;

            if (score > currentScore) {
                currentScore = score;
                currentDevice = physicalDevice;
            }
        }

        return currentDevice;
    }
}