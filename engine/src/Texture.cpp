#include "Texture.h"

namespace Vixen {
    Texture::Texture(const std::shared_ptr<LogicalDevice> &logicalDevice,
                     const std::shared_ptr<PhysicalDevice> &physicalDevice, const std::string &path) {
        /// Load texture from disk
        int32_t width, height, channels;
        stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels)
            throw std::runtime_error("Failed to open image for Texture creation");

        VkDeviceSize size = width * height * channels;
        VkFormat format;
        if (channels == STBI_rgb_alpha)
            format = VK_FORMAT_R8G8B8A8_SRGB;
        else if (channels == STBI_rgb)
            format = VK_FORMAT_R8G8B8_SRGB;
        else
            throw std::runtime_error("Unsupported image format");

        Buffer staging = Buffer(logicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
        staging.write(pixels, size, 0);
        stbi_image_free(pixels);

        image = std::make_shared<Image>(logicalDevice, width, height, format, VK_IMAGE_TILING_OPTIMAL,
                                        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        image->transition(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        image->copyFrom(staging);
        image->transition(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        view = std::make_unique<ImageView>(image, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    const std::shared_ptr<Image> &Texture::getImage() const {
        return image;
    }

    const std::unique_ptr<ImageView> &Texture::getView() const {
        return view;
    }
}