#pragma once

#include "LogicalDevice.h"

namespace Vixen::Vk {
    class ImageSampler {
        const std::shared_ptr<LogicalDevice> device;

        VkSampler sampler;

    public:
        explicit ImageSampler(const std::shared_ptr<LogicalDevice>& device);

        ImageSampler(const ImageSampler &) = delete;

        ~ImageSampler();

        [[nodiscard]] VkSampler getSampler() const;
    };
}
