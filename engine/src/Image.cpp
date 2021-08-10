#include "Image.h"

namespace Vixen {
    Image::Image(const std::shared_ptr<LogicalDevice> &device, uint32_t width, uint32_t height, VkFormat format,
                 VkImageTiling tiling, VkImageUsageFlags usageFlags) : allocation(nullptr), width(width),
                                                                       height(height),
                                                                       layout(VK_IMAGE_LAYOUT_UNDEFINED),
                                                                       usageFlags(usageFlags), device(device),
                                                                       image(nullptr), format(format) {
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
        imageCreateInfo.usage = usageFlags;
        imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VK_CHECK_RESULT(vmaCreateImage(device->allocator, &imageCreateInfo, &allocationCreateInfo, &image, &allocation,
                                       nullptr))
    }

    Image::Image(Image &&other) noexcept: allocation(std::exchange(other.allocation, nullptr)), width(other.width),
                                          height(other.height), layout(other.layout), usageFlags(other.usageFlags),
                                          device(other.device), image(std::exchange(other.image, nullptr)),
                                          format(other.format) {}

    Image::~Image() {
        vmaDestroyImage(device->allocator, image, allocation);
    }

    Image Image::from(const std::shared_ptr<LogicalDevice> &device, const std::string &path) {
        int32_t width, height, channels;
        stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels)
            throw std::runtime_error("Failed to open image");

        VkDeviceSize size = width * height * 4;
        Buffer staging = Buffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
        staging.write(pixels, size, 0);
        stbi_image_free(pixels);

        auto image = Image(device, width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        image.transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        image.copyFrom(staging);
        image.transition(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        return image;
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

        CommandBuffer(device)
                .recordSingleUsage()
                .cmdPipelineBarrier(source, destination, {}, {}, {}, {barrier})
                .submit();

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
        region.imageExtent = {width, height, 1};

        CommandBuffer(device)
                .recordSingleUsage()
                .cmdCopyBufferToImage(buffer.getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, {region})
                .submit();
    }

    std::shared_ptr<LogicalDevice> Image::getDevice() const {
        return device;
    }

    VkFormat Image::getFormat() const {
        return format;
    }

    VkImageUsageFlags Image::getUsageFlags() const {
        return usageFlags;
    }
}
