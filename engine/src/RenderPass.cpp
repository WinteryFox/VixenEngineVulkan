#include "RenderPass.h"

namespace Vixen {
    RenderPass::RenderPass(const std::shared_ptr<LogicalDevice> &device,
                           const std::vector<VkSubpassDescription> &subpasses,
                           const std::vector<VkAttachmentDescription> &attachments) : device(device) {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = attachments.size();
        createInfo.pAttachments = attachments.data();
        createInfo.subpassCount = subpasses.size();
        createInfo.pSubpasses = subpasses.data();
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        VK_CHECK_RESULT(vkCreateRenderPass(device->device, &createInfo, nullptr, &renderPass))
    }

    RenderPass::~RenderPass() {
        vkDestroyRenderPass(device->device, renderPass, nullptr);
    }
}
