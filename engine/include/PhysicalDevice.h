#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include "Instance.h"

namespace vixen {
    class PhysicalDevice {
    public:
        VkPhysicalDevice device = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties deviceProperties{};
        VkPhysicalDeviceDriverPropertiesKHR driverProperties{};
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkQueueFamilyProperties deviceQueueFamily{};
        uint32_t deviceQueueFamilyIndex = 0;

        explicit PhysicalDevice(const Instance &instance);

    private:
        std::pair<VkPhysicalDevice, std::optional<VkQueueFamilyProperties>>
        pickDevice(const std::vector<VkPhysicalDevice> &devices);

        std::optional<VkQueueFamilyProperties> findQueueFamilies(VkPhysicalDevice physicalDevice);
    };
}