#pragma once

#include <vulkan/vulkan.h>
#include "Shader.h"

namespace vixen {
    class Render {
    public:
        /**
         * The view port used by the renderer
         */
        VkViewport viewport = {};

        /**
         * The scissor used by the viewport
         */
        VkRect2D scissor = {};

        /**
         * The pipeline layout used by the renderer
         */
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        /**
         * The render pass object used by the renderer pipeline
         */
        VkRenderPass renderPass = VK_NULL_HANDLE;

        /**
         * The graphics pipeline
         */
        VkPipeline pipeline = VK_NULL_HANDLE;

        /**
         * A list of frame buffers used by this graphics pipeline
         */
        std::vector<VkFramebuffer> swapChainFramebuffers = {};

        /**
         * The logical device this renderer was made by and should be destroyed by
         */
        LogicalDevice *device = nullptr;

        /**
         * Construct a new render pipeline
         *
         * @param device The device to create the pipeline for
         * @param vertex The vertex shader this pipeline will use
         * @param fragment The fragment shader this pipeline will use
         */
        Render(LogicalDevice *device, const Shader &vertex, const Shader &fragment);

        ~Render();
    };
}