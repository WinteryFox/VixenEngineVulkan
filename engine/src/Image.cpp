#include "Image.h"

namespace Vixen {
    Image::Image(const std::shared_ptr<LogicalDevice> &device, uint16_t width, uint16_t height, VkFormat format,
                 VkImageTiling tiling, VkImageUsageFlags usage) : device(device), width(width),
                                                                  height(height), format(format),
                                                                  usage(usage) {
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
        imageCreateInfo.initialLayout = layout;
        imageCreateInfo.usage = usage;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; // TODO: This looks sus

        VK_CHECK_RESULT(vmaCreateImage(device->allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation,
                                       nullptr))
    }

    Image::~Image() {
        vmaDestroyImage(device->allocator, image, allocation);
    }

    void Image::transition(VkImageLayout newLayout) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = layout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // TODO: This looks sus too
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = 0;

        VkPipelineStageFlags source;
        VkPipelineStageFlags destination;

        if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destination = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                   newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destination = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else {
            throw std::runtime_error("Unsupported transition layout");
        }

        CommandBuffer(device).recordAndSubmit([&source, &destination, &barrier](VkCommandBuffer buffer) {
            vkCmdPipelineBarrier(buffer, source, destination, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        });

        layout = newLayout;
    }

    void Image::copyFrom(const Buffer &buffer) {
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

        CommandBuffer(device).recordAndSubmit([this, &buffer, &region](VkCommandBuffer commandBuffer) {
            vkCmdCopyBufferToImage(commandBuffer, buffer.getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                                   &region);
        });
    }

    std::shared_ptr<LogicalDevice> Image::getDevice() const {
        return device;
    }

    VkImage Image::getImage() const {
        return image;
    }

    VmaAllocation Image::getAllocation() const {
        return allocation;
    }

    VkFormat Image::getFormat() const {
        return format;
    }

    VkImageUsageFlags Image::getUsage() const {
        return usage;
    }
}
