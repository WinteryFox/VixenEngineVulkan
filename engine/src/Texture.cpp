#include "Texture.h"

namespace Vixen {
    Texture::Texture(const std::unique_ptr<LogicalDevice> &logicalDevice,
                     const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::string &absolutePath)
            : logicalDevice(logicalDevice) {
        /// Load texture from disk
        int32_t width, height, channels;
        stbi_uc *pixels = stbi_load(absolutePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels)
            error("Failed to load texture " + absolutePath);

        VkDeviceSize size = width * height * channels;
        VkFormat format;
        if (channels == 4)
            format = VK_FORMAT_R8G8B8A8_SRGB;
        else if (channels == 3)
            format = VK_FORMAT_R8G8B8_SRGB;
        else
            error("Unsupported image format for image \"" + absolutePath + "\"");

        /// Create VkImage and VmaAllocation for texture
        VkBuffer stagingBuffer;
        VmaAllocation stagingAllocation;

        createBuffer(logicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, stagingBuffer,
                     stagingAllocation);

        void *data;
        vmaMapMemory(logicalDevice->allocator, stagingAllocation, &data);
        memcpy(data, pixels, static_cast<size_t>(size));
        vmaUnmapMemory(logicalDevice->allocator, stagingAllocation);
        stbi_image_free(pixels);

        createImage(logicalDevice, physicalDevice, width, height, format, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, texture, allocation);
        transitionImageLayout(logicalDevice, texture, format, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(logicalDevice, stagingBuffer, texture, static_cast<uint32_t>(width),
                          static_cast<uint32_t>(height));
        transitionImageLayout(logicalDevice, texture, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);

        /// Create VkImageView for texture
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = texture;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        const auto imageViewCreateResult = vkCreateImageView(logicalDevice->device, &imageViewCreateInfo, nullptr,
                                                             &view);
        if (imageViewCreateResult != VK_SUCCESS)
            error("Failed to create texture view for texture \"" + absolutePath + "\": " +
                  std::to_string(imageViewCreateResult));
    }

    Texture::~Texture() {
        vkDestroyImageView(logicalDevice->device, view, nullptr);
        vmaDestroyImage(logicalDevice->allocator, texture, allocation);
    }

    VkImage Texture::getTexture() const {
        return texture;
    }

    VkImageView Texture::getView() const {
        return view;
    }

    VmaAllocation Texture::getAllocation() const {
        return allocation;
    }
}