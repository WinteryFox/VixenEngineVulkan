#pragma once

#include <vulkan/vulkan.h>
#include <memory>
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
         * The command pool used by this renderer
         */
        VkCommandPool commandPool = VK_NULL_HANDLE;

        /**
         * A list of command buffers associated with the corresponding framebuffer
         */
        std::vector<VkCommandBuffer> commandBuffers;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;

        /**
         * The logical device this renderer was made by and should be destroyed by
         */
        std::shared_ptr<LogicalDevice> device = nullptr;

        /**
         * Construct a new render pipeline
         *
         * @param[in] device The device to create the pipeline for
         * @param[in] vertex The vertex shader this pipeline will use
         * @param[in] fragment The fragment shader this pipeline will use
         */
        Render(const std::shared_ptr<LogicalDevice> &device, const std::shared_ptr<PhysicalDevice> &physicalDevice,
               const Shader &vertex, const Shader &fragment);

        ~Render();

        void render();
    };
}