#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include "Instance.h"

namespace vixen {
    class PhysicalDevice {
    public:
        /**
         * The Vulkan physical device
         */
        VkPhysicalDevice device = VK_NULL_HANDLE;

        /**
         * The Vulkan physical device properties
         */
        VkPhysicalDeviceProperties deviceProperties{};

        /**
         * The Vulkan physical device driver properties
         */
        VkPhysicalDeviceDriverPropertiesKHR driverProperties{};

        /**
         * The Vulkan physical device features
         */
        VkPhysicalDeviceFeatures deviceFeatures{};

        /**
         * The Vulkan physical device graphics queue family index relative to other queue families
         */
        uint32_t graphicsFamilyIndex;

        /**
         * The Vulkan physical device present queue family index relative to other queue families
         */
        uint32_t presentFamilyIndex;

        /**
         * Allocates a physical device for Vulkan use
         *
         * @param[in] instance The Vulkan instance the physical device will be allocated for
         */
        explicit PhysicalDevice(const Instance &instance);

    private:
        /**
         * Automatically pick the best suited Vulkan capable physical device and Vulkan queue family
         *
         * @param[in] devices The list of Vulkan physical devices to pick from
         * @return The Vulkan physical device, can be VK_NULL_HANDLE if no suitable physical device was found
         */
        VkPhysicalDevice pickDevice(const Instance &instance, const std::vector<VkPhysicalDevice> &devices);

        /**
         * Automatically find and set the required graphics queue families for a Vulkan physical device
         *
         * @param[in] physicalDevice The physical device to find the queue families for
         */
        std::pair<std::optional<uint32_t>, std::optional<uint32_t>>
        findQueueFamilies(const Instance &instance, const VkPhysicalDevice &physicalDevice);
    };
}