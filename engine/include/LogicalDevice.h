#pragma once

#include <vulkan/vulkan.h>
#include <set>
#include "PhysicalDevice.h"

namespace vixen {
    class LogicalDevice {
    public:
        /**
         * The Vulkan logical device
         */
        VkDevice device = VK_NULL_HANDLE;

        /**
         * The Vulkan graphics queue family
         */
        VkQueue graphicsQueue = {};

        /**
         * The Vulkan presentation queue family
         */
        VkQueue presentQueue = {};

        /**
         * Creates a new Vulkan logical device
         *
         * @param[in] physicalDevice The Vulkan physical device to make the logical device for
         */
        explicit LogicalDevice(const PhysicalDevice &physicalDevice);

        ~LogicalDevice();
    };
}