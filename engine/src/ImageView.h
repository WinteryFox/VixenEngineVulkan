#pragma once

#include "LogicalDevice.h"
#include "Image.h"

namespace Vixen {
    class ImageView {
        const Logger logger{"ImageView"};

        const std::shared_ptr<Image> image;

        VkImageView view{};

    public:
        explicit ImageView(const std::shared_ptr<Image> &image, VkImageAspectFlags flags);

        ImageView() = delete;

        ImageView(const ImageView &) = delete;

        ImageView &operator=(const ImageView &) = delete;

        ~ImageView();

        [[nodiscard]] const std::shared_ptr<Image> &getImage() const;

        [[nodiscard]] VkImageView getView() const;
    };
}
