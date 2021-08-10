#define VMA_IMPLEMENTATION

#include "LogicalDevice.h"

namespace Vixen::Vk {
    LogicalDevice::LogicalDevice(const std::shared_ptr<Instance> &instance, const std::shared_ptr<Window> &window,
                                 const std::shared_ptr<PhysicalDevice> &physicalDevice) : instance(instance),
                                                                                          physicalDevice(
                                                                                                  physicalDevice),
                                                                                          window(window) {
        std::set<uint32_t> queueFamilies = {physicalDevice->graphicsFamilyIndex, physicalDevice->presentFamilyIndex,
                                            physicalDevice->transferFamilyIndex};
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
        deviceCreateInfo.pEnabledFeatures = &physicalDevice->deviceFeatures;
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDevice->enabledExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = physicalDevice->enabledExtensions.data();

        if (vkCreateDevice(physicalDevice->device, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
            logger.critical("Failed to create logical device");
        logger.trace("Successfully created logical device");

        /// Retrieve the graphics queue handle
        vkGetDeviceQueue(device, physicalDevice->graphicsFamilyIndex, 0, &graphicsQueue);
        logger.trace("Successfully created graphics queue interface");

        /// Retrieve the presentation queue handle
        vkGetDeviceQueue(device, physicalDevice->presentFamilyIndex, 0, &presentQueue);
        logger.trace("Successfully created presentation queue interface");

        vkGetDeviceQueue(device, physicalDevice->transferFamilyIndex, 0, &transferQueue);
        logger.trace("Successfully created memory transfer queue interface");

        SwapChainSupportDetails details = physicalDevice->querySwapChainSupportDetails();
        imageCount = details.capabilities.minImageCount + 1;
        if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
            imageCount = details.capabilities.maxImageCount;

        createSwapchain();

        createImageViews();

        VkCommandPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.queueFamilyIndex = physicalDevice->graphicsFamilyIndex;
        poolCreateInfo.flags = 0;

        if (vkCreateCommandPool(device, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
            logger.critical("Failed to create command pool");
        logger.trace("Successfully created command pool");

        VkCommandPoolCreateInfo transferCommandPoolCreateInfo = {};
        transferCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transferCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        transferCommandPoolCreateInfo.queueFamilyIndex = physicalDevice->transferFamilyIndex;

        if (vkCreateCommandPool(device, &transferCommandPoolCreateInfo, nullptr, &transferCommandPool) !=
            VK_SUCCESS)
            logger.critical("Failed to create memory transfer command pool");
        logger.trace("Successfully created memory transfer command pool");

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(device, &fenceCreateInfo, nullptr, &transferFence) != VK_SUCCESS)
            logger.critical("Failed to create memory transfer fence");

        /// Create the VMA allocator
        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.device = device;
        allocatorCreateInfo.physicalDevice = physicalDevice->device;
        allocatorCreateInfo.instance = instance->getInstance();

        if (vmaCreateAllocator(&allocatorCreateInfo, &allocator) != VK_SUCCESS)
            logger.critical("Failed to create VMA allocator");
    }

    LogicalDevice::~LogicalDevice() {
        vkDeviceWaitIdle(device);

        vkDestroyFence(device, transferFence, nullptr);

        vkDestroyCommandPool(device, commandPool, nullptr);
        vkDestroyCommandPool(device, transferCommandPool, nullptr);
        vmaDestroyAllocator(allocator);

        destroySwapchain();
        vkDestroyDevice(device, nullptr);
    }

    void LogicalDevice::chooseSwapSurfaceFormat() {
        SwapChainSupportDetails details = physicalDevice->querySwapChainSupportDetails();

        for (const auto &form : details.formats)
            if (form.format == VK_FORMAT_B8G8R8A8_SRGB && form.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                surfaceFormat = form;
            else
                surfaceFormat = details.formats[0];
    }

    void LogicalDevice::chooseSwapPresentMode() {
        VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

        for (const auto &mode : physicalDevice->querySwapChainSupportDetails().presentModes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                bestMode = mode;
            else if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
                bestMode = presentMode;

        presentMode = bestMode;
    }

    void LogicalDevice::chooseSwapExtent() {
        SwapChainSupportDetails details = physicalDevice->querySwapChainSupportDetails();

        if (details.capabilities.currentExtent.width !=
            std::numeric_limits<uint32_t>::max()) {
            extent = details.capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(window->window, &width, &height);

            VkExtent2D actualExtent = {
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(details.capabilities.minImageExtent.width,
                                          std::min(
                                                  details.capabilities.maxImageExtent.width,
                                                  actualExtent.width));
            actualExtent.height = std::max(details.capabilities.minImageExtent.height,
                                           std::min(
                                                   details.capabilities.maxImageExtent.height,
                                                   actualExtent.height));

            extent = actualExtent;
        }

        logger.trace("Using swap surface format {} ({}) with present mode {} ({}, {})", surfaceFormat.format,
                     surfaceFormat.colorSpace, presentMode, extent.width, extent.height);
    }

    void LogicalDevice::createImageViews() {
        imageViews.resize(images.size());
        for (size_t i = 0; i < images.size(); i++) {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
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
                logger.critical("Failed to create image views");
        }
    }

    void LogicalDevice::createSwapchain() {
        chooseSwapSurfaceFormat();
        chooseSwapPresentMode();
        chooseSwapExtent();

        VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
        swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCreateInfo.surface = instance->getSurface();
        swapchainCreateInfo.minImageCount = imageCount;
        swapchainCreateInfo.imageFormat = surfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
        swapchainCreateInfo.imageExtent = extent;
        swapchainCreateInfo.imageArrayLayers = 1;
        swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        /// If the graphics family is not the same as the present family, we need to use concurrent sharing mode
        if (physicalDevice->graphicsFamilyIndex != physicalDevice->presentFamilyIndex) {
            uint32_t indices[] = {physicalDevice->graphicsFamilyIndex, physicalDevice->presentFamilyIndex};
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            swapchainCreateInfo.queueFamilyIndexCount = 2;
            swapchainCreateInfo.pQueueFamilyIndices = indices;
        } else {
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.queueFamilyIndexCount = 0;
            swapchainCreateInfo.pQueueFamilyIndices = nullptr;
        }

        /// Image details
        swapchainCreateInfo.preTransform = physicalDevice->querySwapChainSupportDetails().capabilities.currentTransform;
        swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainCreateInfo.presentMode = presentMode;
        swapchainCreateInfo.clipped = VK_TRUE;
        swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

        /// Create the swap chain
        if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS)
            logger.critical("Failed to create swap chain");
        logger.info("Successfully created swap chain");

        /// Create the swap chain image views
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
        images.resize(imageCount);
        vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());
    }

    void LogicalDevice::destroySwapchain() {
        int width = 0, height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window->window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(device);

        for (const auto &imageView : imageViews)
            vkDestroyImageView(device, imageView, nullptr);

        vkDestroySwapchainKHR(device, swapchain, nullptr);
    }
}