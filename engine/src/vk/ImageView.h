#pragma once

#include "Image.h"

namespace Vixen::Vk {
    class ImageView : public Image {
        const Logger logger{"ImageView"};

        VkImageView view{};

    public:
        ImageView(Image &&image, VkImageAspectFlags aspectFlags);

        ImageView(const ImageView &) = delete;

        ImageView &operator=(const ImageView &) = delete;

        ~ImageView();

        [[nodiscard]] VkImageView getView() const;
    };
}
