#include "PhysicalDevice.h"

namespace Vixen {
    PhysicalDevice::PhysicalDevice(const std::unique_ptr<Instance> &instance,
                                   const std::vector<const char *> &extensions)
            : enabledExtensions(extensions), instance(instance) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            logger.critical(
                    "There are no Vulkan supported GPUs available, updating your graphics drivers may fix this.");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance->instance, &deviceCount, devices.data());

        std::string output;
        for (const auto &physicalDevice : devices) {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevice, &properties);
            output += std::string(properties.deviceName) + " (" +
                      std::to_string(VK_VERSION_MAJOR(properties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_MINOR(properties.apiVersion)) + "." +
                      std::to_string(VK_VERSION_PATCH(properties.apiVersion)) + ") ";
        }
        logger.trace("Found {} physical device(s); {}", devices.size(), output);

        device = pickDevice(devices, extensions);
        if (device == VK_NULL_HANDLE) {
            logger.critical("No suitable GPU found.");
            exit(1);
        }

        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES_KHR;
        driverProperties.pNext = nullptr;

        VkPhysicalDeviceProperties2 deviceProperties2;
        deviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProperties2.properties = deviceProperties;
        deviceProperties2.pNext = &driverProperties;

        vkGetPhysicalDeviceProperties2(device, &deviceProperties2);

        logger.info("Allocated a GPU with name {} {} running Vulkan v{}.{}.{}", deviceProperties.deviceName,
                    driverProperties.driverInfo, VK_VERSION_MAJOR(deviceProperties.apiVersion),
                    VK_VERSION_MINOR(deviceProperties.apiVersion), VK_VERSION_PATCH(deviceProperties.apiVersion));
    }

    // TODO: Fix this shit
    VkPhysicalDevice PhysicalDevice::pickDevice(const std::vector<VkPhysicalDevice> &devices,
                                                const std::vector<const char *> &extensions) {
        int currentScore = 0;
        VkPhysicalDevice currentDevice = VK_NULL_HANDLE;

        for (const auto &physicalDevice : devices) {
            int score = 0;

            VkPhysicalDeviceProperties physicalDeviceProperties;
            VkPhysicalDeviceFeatures physicalDeviceFeatures;
            vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
            vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

            if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                score += 1000;

            score += physicalDeviceProperties.limits.maxImageDimension2D;

            std::tuple<std::optional<uint32_t>, std::optional<uint32_t>, std::optional<uint32_t>> tuple =
                    findQueueFamilies(physicalDevice);
            /// If there are any queue families missing, this device is not suitable
            if (!std::get<0>(tuple).has_value() || !std::get<1>(tuple).has_value() || !std::get<2>(tuple).has_value())
                continue;

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> avExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, avExtensions.data());

            std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
            for (const auto &extension : avExtensions)
                requiredExtensions.erase(extension.extensionName);

            /// If not all of the required extensions are available, this device is not suitable
            if (!requiredExtensions.empty())
                continue;

            SwapChainSupportDetails details = querySwapChainSupportDetails(physicalDevice);

            /// If there are no surface formats or present modes available on this device, it is not suitable
            if (details.formats.empty() || details.presentModes.empty())
                continue;

            if (score > 0 && score > currentScore) {
                currentScore = score;
                currentDevice = physicalDevice;
                graphicsFamilyIndex = std::get<0>(tuple).value();
                presentFamilyIndex = std::get<1>(tuple).value();
                transferFamilyIndex = std::get<2>(tuple).value();
            }
        }

        return currentDevice;
    }

    std::tuple<std::optional<uint32_t>, std::optional<uint32_t>, std::optional<uint32_t>>
    PhysicalDevice::findQueueFamilies(const VkPhysicalDevice &physicalDevice) {
        std::optional<uint32_t> graphicsIndex;
        std::optional<uint32_t> presentIndex;
        std::optional<uint32_t> transferIndex;

        /// Get a list of all the queue families on this physical device
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        /// Loop over all of them and check if all the necessary queues are present
        uint32_t i = 0;
        for (const auto &properties : queueFamilyProperties) {
            if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphicsIndex = i;

            if (properties.queueCount > 0 && properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
                transferIndex = i;

            VkBool32 presentSupport = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, instance->surface, &presentSupport);
            if (presentSupport)
                presentIndex = i;

            i++;
        }

        return std::tuple<std::optional<uint32_t>, std::optional<uint32_t>, std::optional<uint32_t>>(graphicsIndex,
                                                                                                     presentIndex,
                                                                                                     transferIndex);
    }

    SwapChainSupportDetails PhysicalDevice::querySwapChainSupportDetails(VkPhysicalDevice physicalDevice) {
        SwapChainSupportDetails details;

        /// Get the surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, instance->surface, &details.capabilities);

        /// Get the supported surface formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, instance->surface, &formatCount, nullptr);

        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, instance->surface, &formatCount,
                                             details.formats.data());

        /// Get the supported present modes
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, instance->surface, &presentModeCount, nullptr);

        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, instance->surface, &presentModeCount,
                                                  details.presentModes.data());

        return details;
    }

    SwapChainSupportDetails PhysicalDevice::querySwapChainSupportDetails() {
        return querySwapChainSupportDetails(device);
    }

    VkFormat PhysicalDevice::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                                 VkFormatFeatureFlags features) const {
        for (const auto &format : candidates) {
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(device, format, &formatProperties);

            if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
                       (formatProperties.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw VulkanException("Failed to find suitable depth image format");
    }
}