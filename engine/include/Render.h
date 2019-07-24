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
        std::vector<VkFramebuffer> framebuffers = {};

        /**
         * The command pool used by this renderer
         */
        VkCommandPool commandPool = VK_NULL_HANDLE;

        /**
         * A list of command buffers associated with the corresponding framebuffer
         */
        std::vector<VkCommandBuffer> commandBuffers;

        /**
         * A list of all the semaphores for the image available signal for GPU-GPU synchronization
         */
        std::vector<VkSemaphore> imageAvailableSemaphores = {};

        /**
         * A list of all the semaphores for the render finished signal for GPU-GPU synchronization
         */
        std::vector<VkSemaphore> renderFinishedSemaphores = {};

        /**
         * A list of all the fences, used for CPU-GPU synchronization
         */
        std::vector<VkFence> fences = {};

        /**
         * The maximum number of frames in flight, also known as concurrently rendered frames
         * ensuring that the GPU is always being utilized
         */
        const int framesInFlight;

        /**
         * The logical device this renderer was made by and should be destroyed by
         */
        std::shared_ptr<LogicalDevice> device = nullptr;

        const Shader vertex;

        const Shader fragment;

        /**
         * Construct a new render pipeline
         *
         * @param[in] device The device to create the pipeline for
         * @param[in] vertex The vertex shader this pipeline will use
         * @param[in] fragment The fragment shader this pipeline will use
         * @param[in] framesInFlight The maximum frames in flight to be used by this renderer
         */
        Render(const std::shared_ptr<LogicalDevice> &device, const std::shared_ptr<PhysicalDevice> &physicalDevice,
               const Shader &vertex, const Shader &fragment, int framesInFlight);

        ~Render();

        /**
         * Renders the current scene
         */
        void render();

        void createCommandBuffers();

        void createFramebuffers();

        void createSyncObjects();

        void createRenderPass();

        void createPipeline();

        void createPipelineLayout();

        void destroyPipeline();

        void recreate();

    private:
        /**
         * The current frame in relation to the maximum frames in flight
         */
        size_t currentFrame = 0;
    };
}