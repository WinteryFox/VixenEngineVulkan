#pragma once

#include <vulkan/vulkan.h>
#include "Instance.h"

namespace vixen {
    class PhysicalDevice {
    public:
        VkPhysicalDevice device;
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;

        explicit PhysicalDevice(const Instance &instance);

        explicit PhysicalDevice(VkPhysicalDevice device);

    private:
        VkPhysicalDevice pickDevice(const std::vector<VkPhysicalDevice> &devices);
    };
}