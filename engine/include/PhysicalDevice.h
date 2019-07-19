#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <set>
#include "Instance.h"

namespace vixen {
    class PhysicalDevice {
    public:
        /**
         * The Vulkan physical device
         */
        VkPhysicalDevice device = VK_NULL_HANDLE;

        /**
         * The Vulkan physical device properties
         */
        VkPhysicalDeviceProperties deviceProperties{};

        /**
         * The Vulkan physical device driver properties
         */
        VkPhysicalDeviceDriverPropertiesKHR driverProperties{};

        /**
         * The Vulkan physical device features
         */
        VkPhysicalDeviceFeatures deviceFeatures{};

        /**
         * A list of all extensions supported by this physical device
         */
        std::vector<VkExtensionProperties> availableExtensions;

        /***
         * The surface format capabilities for this Vulkan physical device
         */
        VkSurfaceCapabilitiesKHR surfaceCapabilities;

        /**
         * The present modes available for this Vulkan physical device
         */
        std::vector<VkPresentModeKHR> availablePresentModes;

        /**
         * A list of currently enabled extensions, this is used when creating the logical device
         */
        std::vector<const char *> enabledExtensions;

        /**
         * The Vulkan physical device graphics queue family index relative to other queue families
         */
        uint32_t graphicsFamilyIndex = 0;

        /**
         * The Vulkan physical device present queue family index relative to other queue families
         */
        uint32_t presentFamilyIndex = 0;

        /**
         * The surface formats available for this Vulkan physical device
         */
        std::vector<VkSurfaceFormatKHR> availableSurfaceFormats;

        /**
         * Allocates a physical device for Vulkan use
         *
         * @param[in] instance The Vulkan instance to pick the device for
         * @param[in] devices The list of Vulkan physical devices to pick from
         * @param[in] extensions The device extensions required by the application
         */
        explicit PhysicalDevice(const Instance &instance,
                                const std::vector<const char *> &extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME});

    private:
        /**
         * Automatically pick the best suited Vulkan capable physical device and Vulkan queue family
         *
         * @param[in] instance The Vulkan instance to pick the device for
         * @param[in] devices The list of Vulkan physical devices to pick from
         * @param[in] extensions The device extensions required by the application
         * @return Returns the Vulkan physical device, can be VK_NULL_HANDLE if no suitable physical device was found
         */
        VkPhysicalDevice pickDevice(const Instance &instance, const std::vector<VkPhysicalDevice> &devices,
                                    const std::vector<const char *> &extensions);

        /**
         * Automatically find and set the required graphics queue families for a Vulkan physical device
         *
         * @param[in] physicalDevice The physical device to find the queue families for
         */
        std::pair<std::optional<uint32_t>, std::optional<uint32_t>>
        findQueueFamilies(const Instance &instance, const VkPhysicalDevice &physicalDevice);
    };
}