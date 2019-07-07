#pragma once

#include <vulkan/vulkan.h>
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
         * Creates a new Vulkan logical device
         *
         * @param physicalDevice The Vulkan physical device to make the logical device for
         */
        explicit LogicalDevice(PhysicalDevice physicalDevice);

        ~LogicalDevice();
    };
}