#pragma once

#include "LogicalDevice.h"

namespace Vixen::Vk {
    class Framebuffer {
        const std::shared_ptr<LogicalDevice> logicalDevice;

        VkFramebuffer framebuffer{};

    public:
        Framebuffer(const std::shared_ptr<LogicalDevice> &logicalDevice, VkRenderPass renderPass,
                    const std::vector<VkImageView> &attachments, int width, int height);

        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        ~Framebuffer();

        [[nodiscard]] VkFramebuffer getFramebuffer() const;
    };
}
