#pragma once

#include <vulkan/vulkan.h>
#include "VulkanException.h"
#include "LogicalDevice.h"

#ifdef VIXEN_DEBUG
#define VK_CHECK_RESULT(f) { \
    VkResult result = (f); \
    if (result != VK_SUCCESS) { \
        throw Vixen::VulkanException("VkResult is \"" + Vixen::errorString(result) + "\" (" + std::to_string(result) \
        + ") " + "at " + __FILE__ + ":" + std::to_string(__LINE__)); \
    } \
}
#else
#define VK_CHECK_RESULT(f) (f);
#endif

namespace Vixen {
    std::string errorString(VkResult result);

    VkImageView
    createImageView(const std::unique_ptr<LogicalDevice> &logicalDevice, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags);

    void
    createImage(const std::unique_ptr<LogicalDevice> &logicalDevice,
                const std::unique_ptr<PhysicalDevice> &physicalDevice,
                uint32_t width, uint32_t height, VkFormat format,
                VkImageTiling tiling, VkImageUsageFlags usage, VkImage &image, VmaAllocation &allocation);

    bool
    createBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
                 VmaMemoryUsage vmaUsage, VkBuffer &buffer, VmaAllocation &allocation);

    VkCommandBuffer beginSingleUseCommandBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice);

    void
    endSingleUseCommandBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkCommandBuffer commandBuffer);

    void copyBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &src, VkBuffer &dst,
                    VkDeviceSize size);

    void
    transitionImageLayout(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkImage &image, VkFormat format,
                          VkImageLayout oldLayout, VkImageLayout newLayout);

    void
    copyBufferToImage(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &buffer, const VkImage &image,
                      uint32_t width, uint32_t height);
}
