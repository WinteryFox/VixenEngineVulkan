#pragma once

#include "Image.h"

namespace Vixen {
    class ImageView : public Image {
        const Logger logger{"ImageView"};

        VkImageView view{};

    public:
        ImageView(const std::shared_ptr<LogicalDevice> &device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);

        ImageView() = delete;

        ImageView(const ImageView &) = delete;

        ImageView &operator=(const ImageView &) = delete;

        ~ImageView();

        [[nodiscard]] VkImageView getView() const;
    };
}
