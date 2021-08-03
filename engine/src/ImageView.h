#pragma once

#include "LogicalDevice.h"
#include "Image.h"

namespace Vixen {
    class ImageView {
        const std::shared_ptr<Image> image;

        VkImageView view{};

    public:
        explicit ImageView(const std::shared_ptr<Image> &image, VkImageAspectFlags flags);

        ImageView(const ImageView &other) = delete;

        ~ImageView();

        [[nodiscard]] const std::shared_ptr<Image> &getImage() const;

        [[nodiscard]] VkImageView getView() const;
    };
}
