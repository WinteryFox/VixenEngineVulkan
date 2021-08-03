#include "ImageView.h"

namespace Vixen {
    ImageView::ImageView(const std::shared_ptr<Image> &image, VkImageAspectFlags flags) : image(image) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = image->getImage();
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = image->getFormat();
        imageViewCreateInfo.subresourceRange.aspectMask = flags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK_RESULT(vkCreateImageView(image->getDevice()->device, &imageViewCreateInfo, nullptr, &view))
    }

    ImageView::~ImageView() {
        vkDestroyImageView(image->getDevice()->device, view, nullptr);
    }

    const std::shared_ptr<Image> &ImageView::getImage() const {
        return image;
    }

    VkImageView ImageView::getView() const {
        return view;
    }
}
