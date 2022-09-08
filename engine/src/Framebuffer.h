#pragma once

#include "LogicalDevice.h"
#include "RenderPass.h"

namespace Vixen {
    class Framebuffer {
        const std::shared_ptr<LogicalDevice> logicalDevice;

        VkFramebuffer framebuffer{};

    public:
        Framebuffer(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::shared_ptr<RenderPass> &renderPass,
                    const std::vector<VkImageView> &attachments, int width, int height);

        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        ~Framebuffer();

        [[nodiscard]] VkFramebuffer getFramebuffer() const;
    };
}
