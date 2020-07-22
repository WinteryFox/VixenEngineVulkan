#include "Framebuffer.h"

namespace Vixen {
    Framebuffer::Framebuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkRenderPass renderPass,
                             const std::vector<VkImageView> &attachments, int width,
                             int height) : logicalDevice(logicalDevice) {
        VkFramebufferCreateInfo framebufferCreateInfo{};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.attachmentCount = attachments.size();
        framebufferCreateInfo.pAttachments = attachments.data();
        framebufferCreateInfo.width = width;
        framebufferCreateInfo.height = height;
        framebufferCreateInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(logicalDevice->device, &framebufferCreateInfo, nullptr, &framebuffer);
        if (result != VK_SUCCESS)
            error(VulkanException("Failed to create a frame buffer", result));
    }

    Framebuffer::~Framebuffer() {
        vkDestroyFramebuffer(logicalDevice->device, framebuffer, nullptr);
    }

    VkFramebuffer Framebuffer::getFramebuffer() const {
        return framebuffer;
    }
}