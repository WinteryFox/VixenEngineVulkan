#include "Texture.h"

namespace Vixen {
    Texture::Texture(const std::unique_ptr<LogicalDevice> &logicalDevice,
                     const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::string &path)
            : logicalDevice(logicalDevice) {
        /// Load texture from disk
        int32_t width, height, channels;
        stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels)
            throw IOException("Failed to open image for Texture creation", path);

        VkDeviceSize size = width * height * channels;
        VkFormat format;
        if (channels == STBI_rgb_alpha)
            format = VK_FORMAT_R8G8B8A8_SRGB;
        else
            throw ImageException(VulkanException("Unsupported image format, has " + std::to_string(channels) +
                                                 " channels", VK_ERROR_FORMAT_NOT_SUPPORTED), path);

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

        try {
            createImage(logicalDevice, physicalDevice, width, height, format, VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, texture, allocation);
        } catch (const VulkanException &exception) {
            throw ImageException(exception, path);
        }
        transitionImageLayout(logicalDevice, texture, format, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(logicalDevice, stagingBuffer, texture, static_cast<uint32_t>(width),
                          static_cast<uint32_t>(height));
        transitionImageLayout(logicalDevice, texture, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);

        try {
            view = createImageView(logicalDevice, texture, format, VK_IMAGE_ASPECT_COLOR_BIT);
        } catch (const VulkanException &exception) {
            throw ImageException(exception, path);
        }
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