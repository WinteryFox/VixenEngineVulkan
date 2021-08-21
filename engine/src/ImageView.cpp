#include "ImageView.h"

namespace Vixen {
    ImageView::ImageView(Image &&image, VkImageAspectFlags aspectFlags)
            : Image(std::move(image)) {
        VkImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.image = this->image;
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = format;
        imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        VK_CHECK_RESULT(vkCreateImageView(device->device, &imageViewCreateInfo, nullptr, &view))
    }

    ImageView::~ImageView() {
        vkDestroyImageView(device->device, view, nullptr);
    }

    VkImageView ImageView::getView() const {
        return view;
    }
}
