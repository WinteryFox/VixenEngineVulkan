#ifndef VIXENENGINE_FRAMEBUFFER_H
#define VIXENENGINE_FRAMEBUFFER_H

#include "LogicalDevice.h"

namespace Vixen {
    class Framebuffer {
        const std::unique_ptr<LogicalDevice> &logicalDevice;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;

    public:
        Framebuffer() = delete;

        Framebuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkRenderPass renderPass,
                    const std::vector<VkImageView> &attachments, int width, int height);

        Framebuffer(Framebuffer&&) = default;

        ~Framebuffer();

        [[nodiscard]] VkFramebuffer getFramebuffer() const;
    };
}

#endif