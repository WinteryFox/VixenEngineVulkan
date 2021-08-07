#pragma once

#include <memory>
#include "Vulkan.h"
#include "Shader.h"
#include "Mesh.h"
#include "Scene.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "DescriptorSetLayout.h"
#include "DescriptorPool.h"

namespace Vixen {
    enum class BufferType {
        SINGLE_BUFFER = 1,
        DOUBLE_BUFFER = 2,
        TRIPLE_BUFFER = 3
    };

    class Render {
        const Logger logger{"Render"};

        /**
         * The logical device this renderer was made by and should be destroyed by
         */
        const std::shared_ptr<LogicalDevice> logicalDevice;

        const std::shared_ptr<PhysicalDevice> physicalDevice;

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
        std::vector<std::shared_ptr<Framebuffer>> framebuffers = {};

        /**
         * A list of command buffers associated with the corresponding framebuffer
         */
        std::vector<std::shared_ptr<CommandBuffer>> commandBuffers = {};

        /**
         * A list of all the semaphores for the image available signal for GPU-GPU synchronization
         */
        std::vector<VkSemaphore> imageAvailableSemaphores = {};

        /**
         * A list of all the semaphores for the render finished signal for GPU-GPU synchronization
         */
        std::vector<VkSemaphore> renderFinishedSemaphores = {};

        std::shared_ptr<DescriptorPool> descriptorPool = nullptr;

        std::unique_ptr<DescriptorSetLayout> descriptorSetLayout = nullptr;

        std::vector<std::unique_ptr<Buffer>> uniformBuffers;

        std::vector<std::vector<VkDescriptorSet>> descriptorSet;

        VkSampler textureSampler{};

        std::unique_ptr<ImageView> depthImage{};

        /**
         * The maximum number of frames in flight, also known as concurrently rendered frames
         * ensuring that the GPU is always being utilized
         */
        const uint8_t framesInFlight;

        const std::shared_ptr<const Shader> shader;

        const Scene &scene;

        double lastTime = glfwGetTime();

        double deltaTime{};

        /**
         * The current frame in relation to the maximum frames in flight
         */
        uint32_t currentFrame = 0;

        void createDepthImage();

        void destroyDepthImage();

        void createCommandBuffers();

        void destroyFramebuffers();

        void createSyncObjects();

        void destroySyncObjects();

        void createRenderPass();

        void destroyRenderPass();

        void createPipeline();

        void destroyPipeline();

        void createPipelineLayout();

        void destroyPipelineLayout();

        void createUniformBuffers();

        void createSampler();

        void destroySampler();

        std::vector<std::vector<VkDescriptorSet>> createDescriptorSets();

        void invalidate();

        void create();

        void destroy();

        void updateUniformBuffer(const std::unique_ptr<Camera> &camera, const Entity &entity, uint32_t imageIndex);

    public:
        /**
         * Construct a new render pipeline
         *
         * @param[in] device The device to create the pipeline for
         * @param[in] vertex The vertex shader this pipeline will use
         * @param[in] fragment The fragment shader this pipeline will use
         * @param[in] framesInFlight The maximum frames in flight to be used by this renderer
         */
        Render(std::shared_ptr<LogicalDevice> device, std::shared_ptr<PhysicalDevice> physicalDevice,
               const Scene &scene, std::shared_ptr<const Shader> shader,
               BufferType bufferType = BufferType::DOUBLE_BUFFER);

        ~Render();

        /**
         * Renders the current scene
         */
        void render(const std::unique_ptr<Camera> &camera);

        [[nodiscard]] double getLastTime() const;

        [[nodiscard]] double getDeltaTime() const;
    };
}