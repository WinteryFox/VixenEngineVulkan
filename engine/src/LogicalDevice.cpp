#include "LogicalDevice.h"

namespace vixen {
    LogicalDevice::LogicalDevice(PhysicalDevice physicalDevice) {
        float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = physicalDevice.deviceQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &physicalDevice.deviceFeatures;

        if (vkCreateDevice(physicalDevice.device, &createInfo, nullptr, &device) != VK_SUCCESS)
            fatal("Failed to create logical device");
        info("Successfully created logical device");

        vkGetDeviceQueue(device, physicalDevice.deviceQueueFamilyIndex, 0, &graphicsQueue);
        info("Successfully created logical device graphics queue interface");
    }

    LogicalDevice::~LogicalDevice() {
        vkDestroyDevice(device, nullptr);
    }
}