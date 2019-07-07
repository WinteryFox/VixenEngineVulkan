#include "LogicalDevice.h"

namespace vixen {
    LogicalDevice::LogicalDevice(const PhysicalDevice &physicalDevice) {
        std::set<uint32_t> queueFamilies = {physicalDevice.graphicsFamilyIndex, physicalDevice.presentFamilyIndex};

        /// Create all the queue create info structs specified in queueFamilies
        float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        for (uint32_t queueFamily : queueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo = {};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &physicalDevice.deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDevice.enabledExtensions.size());
        createInfo.ppEnabledExtensionNames = physicalDevice.enabledExtensions.data();

        if (vkCreateDevice(physicalDevice.device, &createInfo, nullptr, &device) != VK_SUCCESS)
            fatal("Failed to create logical device");
        trace("Successfully created logical device");

        /// Retrieve the graphics queue handle
        vkGetDeviceQueue(device, physicalDevice.graphicsFamilyIndex, 0, &graphicsQueue);
        trace("Successfully created graphics queue interface");

        /// Retrieve the presentation queue handle
        vkGetDeviceQueue(device, physicalDevice.presentFamilyIndex, 0, &presentQueue);
        trace("Successfully created presentation queue interface");
    }

    LogicalDevice::~LogicalDevice() {
        vkDestroyDevice(device, nullptr);
    }
}