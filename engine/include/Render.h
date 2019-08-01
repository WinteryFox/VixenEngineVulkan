#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "Shader.h"
#include "Mesh.h"
#include "Scene.h"

namespace Vixen {
    class Render {
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
        std::vector<VkCommandBuffer> commandBuffers = {};

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

        VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

        std::vector<VkBuffer> uniformBuffers;

        std::vector<VmaAllocation> uniformBuffersMemory;

        /**
         * The maximum number of frames in flight, also known as concurrently rendered frames
         * ensuring that the GPU is always being utilized
         */
        const int framesInFlight;

        const std::unique_ptr<Shader> &vertex;

        const std::unique_ptr<Shader> &fragment;

        const Scene &scene;

        /**
         * The logical device this renderer was made by and should be destroyed by
         */
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        const std::unique_ptr<PhysicalDevice> &physicalDevice;

        /**
         * The current frame in relation to the maximum frames in flight
         */
        size_t currentFrame = 0;

        void createCommandBuffers();

        void destroyCommandBuffers();

        void createFramebuffers();

        void destroyFramebuffers();

        void createSyncObjects();

        void destroySyncObjects();

        void createRenderPass();

        void destroyRenderPass();

        void createPipeline();

        void destroyPipeline();

        void createPipelineLayout();

        void destroyPipelineLayout();

        void createDescriptorSetLayout();

        void destroyDescriptorSetLayout();

        void createUniformBuffers();

        void destroyUniformBuffers();

        void createCommandPool();

        void destroyCommandPool();

        void invalidate();

        void updateUniformBuffer(Entity entity, uint32_t imageIndex);

    public:
        /**
         * Construct a new render pipeline
         *
         * @param[in] device The device to create the pipeline for
         * @param[in] vertex The vertex shader this pipeline will use
         * @param[in] fragment The fragment shader this pipeline will use
         * @param[in] framesInFlight The maximum frames in flight to be used by this renderer
         */
        Render(const std::unique_ptr<LogicalDevice> &device, const std::unique_ptr<PhysicalDevice> &physicalDevice,
               const Scene &scene, const std::unique_ptr<Shader> &vertex, const std::unique_ptr<Shader> &fragment,
               int framesInFlight);

        ~Render();

        /**
         * Renders the current scene
         */
        void render();
    };
}