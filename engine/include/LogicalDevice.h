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
         * The Vulkan swap chain
         */
        VkSwapchainKHR swapChain = VK_NULL_HANDLE;

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
        VkExtent2D extent;

        /**
         * The minimum amount of images to be in the swap chain
         */
        uint32_t imageCount;

        /**
         * The surface format this Vulkan physical device is currently using
         */
        VkSurfaceFormatKHR surfaceFormat;

        /**
         * The present mode this Vulkan physical device is currently using
         */
        VkPresentModeKHR presentMode;

        /**
         * All of the images in the current swap chain
         */
        std::vector<VkImage> swapChainImages;

        /**
         * Creates a new Vulkan logical device
         *
         * @param[in] instance The Vulkan instance to create the logical device for
         * @param[in] physicalDevice The Vulkan physical device to make the logical device for
         */
        explicit LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice);

        ~LogicalDevice();

    private:
        /**
         * Pick the best surface format from the available formats
         *
         * @param[in] availableFormats The available formats on this device
         * @return Returns the optimal swap surface format
         */
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

        /**
         * Pick the best present mode from the available formats
         *
         * @param[in] availablePresentModes The available present modes on this device
         * @return Returns the optimal present mode
         */
        VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

        VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
    };
}