#include "Framebuffer.h"

namespace Vixen::Vk {
    Framebuffer::Framebuffer(const std::shared_ptr<LogicalDevice> &logicalDevice, VkRenderPass renderPass,
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

        VK_CHECK_RESULT(vkCreateFramebuffer(logicalDevice->device, &framebufferCreateInfo, nullptr, &framebuffer))
    }

    Framebuffer::~Framebuffer() {
        vkDestroyFramebuffer(logicalDevice->device, framebuffer, nullptr);
    }

    VkFramebuffer Framebuffer::getFramebuffer() const {
        return framebuffer;
    }
}