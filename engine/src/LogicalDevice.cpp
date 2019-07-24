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

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures = &physicalDevice.deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDevice.enabledExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = physicalDevice.enabledExtensions.data();

        if (vkCreateDevice(physicalDevice.device, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
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

        /// Define the swap chain image format
        swapChainFormat = surfaceFormat.format;

        /// Create the swap chain info struct
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = instance.surface;
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = swapChainFormat;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        /// If the graphics family is not the same as the present family, we need to use concurrent sharing mode
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

        /// Image details
        swapchainCreateInfo.preTransform = physicalDevice.surfaceCapabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        /// Create the swap chain
        if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
            fatal("Failed to create swap chain");
        }
        info("Successfully created initial swap chain");

        /// Create the swap chain image views
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapChainImages.data());

        swapChainImageViews = createImageViews(swapChainImages);
    }

    LogicalDevice::~LogicalDevice() {
        for (const auto &view : swapChainImageViews)
            vkDestroyImageView(device, view, nullptr);

        vkDestroySwapchainKHR(device, swapchain, nullptr);
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

    std::vector<VkImageView> LogicalDevice::createImageViews(const std::vector<VkImage> &images) {
        std::vector<VkImageView> imageViews(images.size());

        for (uint32_t i = 0; i < images.size(); i++) {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapChainFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS)
                fatal("Failed to create image views");
        }

        return imageViews;
    }

    void LogicalDevice::destroySwapchain() {
        for (const auto &imageView : swapChainImageViews)
            vkDestroyImageView(device, imageView, nullptr);

        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
}