#include "LogicalDevice.h"

namespace vixen {
    LogicalDevice::LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice) {
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

        /// Automatically choose the surface format, swap present mode and swap extent to be used
        surfaceFormat = chooseSwapSurfaceFormat(physicalDevice.availableSurfaceFormats);
        presentMode = chooseSwapPresentMode(physicalDevice.availablePresentModes);
        extent = chooseSwapExtent(physicalDevice.surfaceCapabilities);
        trace("Using the following swap surface format and swap present mode; " + std::to_string(surfaceFormat.format) +
              "(" + std::to_string(surfaceFormat.colorSpace) + ")" + ", " + std::to_string(presentMode) +
              " with the following extent " + std::to_string(extent.width) + ", " + std::to_string(extent.height));

        imageCount = physicalDevice.surfaceCapabilities.minImageCount + 1;
        if (physicalDevice.surfaceCapabilities.minImageCount > 0 &&
            imageCount > physicalDevice.surfaceCapabilities.maxImageCount)
            imageCount = physicalDevice.surfaceCapabilities.maxImageCount;

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = instance.surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        if (physicalDevice.graphicsFamilyIndex != physicalDevice.presentFamilyIndex) {
            uint32_t indices[] = {physicalDevice.graphicsFamilyIndex, physicalDevice.presentFamilyIndex};
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = indices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        swapchainCreateInfo.preTransform = physicalDevice.surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapChain) != VK_SUCCESS) {
            fatal("Failed to create swap chain");
        }
        info("Successfully created initial swap chain");

        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
    }

    LogicalDevice::~LogicalDevice() {
        vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
    }

    VkSurfaceFormatKHR
    LogicalDevice::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats) {
        for (const auto &format : availableFormats)
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;

        return availableFormats[0];
    }

    VkPresentModeKHR LogicalDevice::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availableModes) {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto &mode : availableModes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
            else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                bestMode = presentMode;

        return bestMode;
    }

    VkExtent2D LogicalDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            VkExtent2D actualExtent;

            actualExtent.width = std::max(capabilities.minImageExtent.width,
                                          std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height,
                                           std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }
}