#pragma once

#include <vulkan/vulkan.h>
#include <set>
#include <memory>
#include "PhysicalDevice.h"

namespace vixen {
    class LogicalDevice {
    public:
        /**
         * The Vulkan logical device
         */
        VkDevice device = VK_NULL_HANDLE;

        /**
         * The Vulkan swap chain
         */
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;

        /**
         * The Vulkan graphics queue family
         */
        VkQueue graphicsQueue = {};

        /**
         * The Vulkan presentation queue family
         */
        VkQueue presentQueue = {};

        /**
         * The extent currently being used by this Vulkan physical device
         */
        VkExtent2D extent = {};

        /**
         * The minimum amount of images to be in the swap chain
         */
        uint32_t imageCount = 0;

        /**
         * The surface format this Vulkan physical device is currently using
         */
        VkSurfaceFormatKHR surfaceFormat = {};

        /**
         * The present mode this Vulkan physical device is currently using
         */
        VkPresentModeKHR presentMode = {};

        /**
         * All of the images in the current swap chain
         */
        std::vector<VkImage> images;

        /**
         * All of the swap chain image views
         */
        std::vector<VkImageView> imageViews;

        /**
         * A pointer to the instance this logical device was created by
         */
        const std::unique_ptr<Instance> &instance;

        /**
         * A pointer to the physical device this logical device was created by
         */
        const std::unique_ptr<PhysicalDevice> &physicalDevice;

        /**
         * A pointer to the window this device will use
         */
        const std::unique_ptr<Window> &window;

        /**
         * Creates a new Vulkan logical device
         *
         * @param[in] instance The Vulkan instance to create the logical device for
         * @param[in] physicalDevice The Vulkan physical device to make the logical device for
         */
        LogicalDevice(const std::unique_ptr<Instance> &instance,
                      const std::unique_ptr<Window> &window,
                      const std::unique_ptr<PhysicalDevice> &physicalDevice);

        ~LogicalDevice();

        /**
         * Pick the best surface format from the available formats
         */
        void chooseSwapSurfaceFormat();

        /**
         * Pick the best present mode from the available formats
         */
        void chooseSwapPresentMode();

        void chooseSwapExtent();

        /**
         * Creates the image views for a vector of images
         */
        void createImageViews();

        void createSwapchain();

        void destroySwapchain();
    };
}