#include "Vulkan.h"

namespace Vixen {
    std::string errorString(VkResult result) {
        switch (result) {
#define STR(r) case VK_ ##r: return #r
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_FEATURE_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            STR(ERROR_TOO_MANY_OBJECTS);
            STR(ERROR_FORMAT_NOT_SUPPORTED);
            STR(ERROR_SURFACE_LOST_KHR);
            STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            STR(SUBOPTIMAL_KHR);
            STR(ERROR_OUT_OF_DATE_KHR);
            STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            STR(ERROR_VALIDATION_FAILED_EXT);
            STR(ERROR_INVALID_SHADER_NV);
            STR(ERROR_OUT_OF_POOL_MEMORY);
#undef STR
            default:
                return "UNKNOWN_ERROR";
        }
    }

    VkImageView
    createImageView(const std::unique_ptr<LogicalDevice> &logicalDevice, VkImage image, VkFormat format,
                    VkImageAspectFlags aspectFlags) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VkImageView view;
        VK_CHECK_RESULT(vkCreateImageView(logicalDevice->device, &imageViewCreateInfo, nullptr, &view))
        return view;
    }

    void
    createImage(const std::unique_ptr<LogicalDevice> &logicalDevice,
                const std::unique_ptr<PhysicalDevice> &physicalDevice,
                const uint32_t width, const uint32_t height, const VkFormat format,
                const VkImageTiling tiling, const VkImageUsageFlags usage, VkImage &image, VmaAllocation &allocation) {
        VkImageCreateInfo imageCreateInfo{};
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
        imageCreateInfo.extent.width = width;
        imageCreateInfo.extent.height = height;
        imageCreateInfo.extent.depth = 1;
        imageCreateInfo.mipLevels = 1;
        imageCreateInfo.arrayLayers = 1;
        imageCreateInfo.format = format;
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VK_CHECK_RESULT(
                vmaCreateImage(logicalDevice->allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation,
                               nullptr))
    }

    /**
     * Create and allocate a new buffer, it is up to the user to delete the buffer and free the device memory
     *
     * @param[in] logicalDevice The logical device to create this buffer on
     * @param[in] size The size to allocate
     * @param[in] usage How this buffer will be used
     * @param[out] buffer The buffer to write to
     * @param[out] allocation The buffer memory allocated to by VMA
     * @return Returns true on successful creation and allocation of the buffer
     * @see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferUsageFlagBits.html
     */
    bool
    createBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
                 VmaMemoryUsage vmaUsage, VkBuffer &buffer, VmaAllocation &allocation) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = vmaUsage;

        VK_CHECK_RESULT(vmaCreateBuffer(logicalDevice->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer,
                                        &allocation, nullptr))
        return true;
    }

    VkCommandBuffer beginSingleUseCommandBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice) {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = logicalDevice->commandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void
    endSingleUseCommandBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkCommandBuffer commandBuffer) {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(logicalDevice->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(logicalDevice->graphicsQueue);

        vkFreeCommandBuffers(logicalDevice->device, logicalDevice->commandPool, 1, &commandBuffer);
    }

    /**
     * Copy the data from one buffer to another
     *
     * @param[in] logicalDevice The device to use for the copy
     * @param[in] src The source buffer
     * @param[out] dst The destination buffer
     * @param[in] size The size of the source buffer
     */
    void copyBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &src, VkBuffer &dst,
                    VkDeviceSize size) {
        VkCommandBuffer commandBuffer = beginSingleUseCommandBuffer(logicalDevice);

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        endSingleUseCommandBuffer(logicalDevice, commandBuffer);
    }

    void transitionImageLayout(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkImage &image,
                               const VkFormat format, const VkImageLayout oldLayout,
                               const VkImageLayout newLayout) {
        VkCommandBuffer commandBuffer = beginSingleUseCommandBuffer(logicalDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        VkPipelineStageFlags source;
        VkPipelineStageFlags destination;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            error("Unsupported layout transition from " + std::to_string(oldLayout) + " to " +
                  std::to_string(newLayout));
        }

        vkCmdPipelineBarrier(
                commandBuffer,
                source,
                destination,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &barrier
        );

        endSingleUseCommandBuffer(logicalDevice, commandBuffer);
    }

    void
    copyBufferToImage(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &buffer, const VkImage &image,
                      const uint32_t width, const uint32_t height) {
        VkCommandBuffer commandBuffer = beginSingleUseCommandBuffer(logicalDevice);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {
                width,
                height,
                1
        };

        vkCmdCopyBufferToImage(
                commandBuffer,
                buffer,
                image,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
        );

        endSingleUseCommandBuffer(logicalDevice, commandBuffer);
    }
}
