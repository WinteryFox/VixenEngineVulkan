#pragma once

#include <vulkan/vulkan.h>
#include "Instance.h"

namespace vixen {
    class PhysicalDevice {
    public:
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceDriverPropertiesKHR driverProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        explicit PhysicalDevice(const Instance &instance);

    private:
        VkPhysicalDevice pickDevice(const std::vector<VkPhysicalDevice> &devices);
    };
}