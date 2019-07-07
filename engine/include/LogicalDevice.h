#pragma once

#include <vulkan/vulkan.h>
#include "PhysicalDevice.h"

namespace vixen {
    class LogicalDevice {
    public:
        VkDevice device;
        VkQueue graphicsQueue;

        explicit LogicalDevice(PhysicalDevice physicalDevice);

        ~LogicalDevice();

    private:

    };
}