#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "LogicalDevice.h"

namespace Vixen {
    class RenderPass {
        const std::shared_ptr<LogicalDevice> &device;

    public:
        VkRenderPass renderPass{};

        RenderPass(const std::shared_ptr<LogicalDevice> &device, const std::vector<VkSubpassDescription> &subpasses,
                   const std::vector<VkAttachmentDescription> &attachments);

        RenderPass(const RenderPass &) = delete;

        RenderPass &operator=(const RenderPass &) = delete;

        ~RenderPass();
    };
}
