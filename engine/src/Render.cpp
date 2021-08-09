#include "Render.h"

namespace Vixen {
    Render::Render(std::shared_ptr<LogicalDevice> device, std::shared_ptr<PhysicalDevice> physicalDevice,
                   const Scene &scene, std::shared_ptr<const Shader> shader, BufferType bufferType)
            : logicalDevice(std::move(device)), physicalDevice(std::move(physicalDevice)),
              framesInFlight(static_cast<const int>(bufferType)),
              shader(std::move(shader)), scene(scene) {
        create();
    }

    Render::~Render() {
        destroy();
    }

    void Render::render(const Camera &camera) {
        commandBuffers[currentFrame]->wait();
        double currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(logicalDevice->device, logicalDevice->swapchain,
                                                std::numeric_limits<uint64_t>::max(),
                                                imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            invalidate();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            logger.critical("Failed to acquire image {}", errorString(result));
        }
        updateUniformBuffer(camera, scene.entities[0], imageIndex);

        commandBuffers[imageIndex]->submit({imageAvailableSemaphores[currentFrame]},
                                           {renderFinishedSemaphores[currentFrame]},
                                           {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT});

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &logicalDevice->swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(logicalDevice->presentQueue, &presentInfo);
        currentFrame = (currentFrame + 1) % framesInFlight;
    }

    void Render::updateUniformBuffer(const Camera &camera, const Entity &entity, uint32_t imageIndex) {
        void *data = uniformBuffers[imageIndex].map();

        glm::mat4 model = entity.getModelMatrix();
        glm::mat4 view = camera.getView();
        glm::mat4 projection = camera.getProjection(static_cast<float>(logicalDevice->extent.width)
                                                     / static_cast<float>(logicalDevice->extent.height));
        projection[1][1] *= -1.0f;

        memcpy(data, &model, sizeof(glm::mat4));
        memcpy((glm::mat4 *) data + 1, &view, sizeof(glm::mat4));
        memcpy((glm::mat4 *) data + 2, &projection, sizeof(glm::mat4));

        uniformBuffers[imageIndex].unmap();
    }

    void Render::destroyFramebuffers() {
        framebuffers.clear();
    }

    void Render::createSyncObjects() {
        /// Create fences and semaphores
        imageAvailableSemaphores.resize(framesInFlight);
        renderFinishedSemaphores.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        for (size_t i = 0; i < framesInFlight; i++)
            if (vkCreateSemaphore(logicalDevice->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(logicalDevice->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS)
                logger.critical("Failed to create semaphores and fences");
        logger.trace("Successfully created semaphores and fences");
    }

    void Render::destroySyncObjects() {
        for (size_t i = 0; i < framesInFlight; i++) {
            vkDestroySemaphore(logicalDevice->device, imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice->device, renderFinishedSemaphores[i], nullptr);
        }
        logger.trace("Destroyed sync objects");
    }

    void Render::createCommandBuffers() {
        framebuffers.reserve(logicalDevice->imageViews.size());
        commandBuffers.reserve(logicalDevice->imageViews.size());
        for (uint32_t i = 0; i < logicalDevice->imageViews.size(); i++) {
            framebuffers.push_back(std::make_shared<Framebuffer>(logicalDevice, renderPass,
                                                                 std::vector<VkImageView>{
                                                                         logicalDevice->imageViews[i],
                                                                         depthImage->getView()
                                                                 },
                                                                 logicalDevice->extent.width,
                                                                 logicalDevice->extent.height));
            commandBuffers.push_back(std::make_shared<CommandBuffer>(logicalDevice));
            auto &commandBuffer = commandBuffers[i];
            commandBuffer->recordSimultaneous();

            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderPass;
            renderPassBeginInfo.framebuffer = framebuffers[i]->getFramebuffer();
            renderPassBeginInfo.renderArea.offset = {0, 0};
            renderPassBeginInfo.renderArea.extent = logicalDevice->extent;

            std::array<VkClearValue, 2> clearColors{};
            //clearColors[0].color = {{34.0f, 59.0f, 84.0f, 1.0f}};
            clearColors[0].color = {{0.13f, 0.23f, 0.33f, 1.0f}};
            clearColors[1].depthStencil = {1.0f, 0};

            renderPassBeginInfo.clearValueCount = clearColors.size();
            renderPassBeginInfo.pClearValues = clearColors.data();

            commandBuffer->cmdBeginRenderPass(renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
            commandBuffer->cmdBindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            for (size_t j = 0; j < scene.entities.size(); j++) {
                const auto &entity = scene.entities[j];
                const auto &mesh = entity.mesh;

                commandBuffer->cmdBindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
                                                     {descriptorSet[i][j]}, {});

                const std::vector<VkBuffer> buffers(3, mesh->getBuffer()->getBuffer());
                std::vector<VkDeviceSize> offsets{0, mesh->getVertexCount() * sizeof(glm::vec3),
                                                  mesh->getVertexCount() * sizeof(glm::vec3) +
                                                  mesh->getVertexCount() * sizeof(glm::vec2)};
                commandBuffer->cmdBindVertexBuffers(0, buffers, offsets);
                commandBuffer->cmdBindIndexBuffer(mesh->getBuffer()->getBuffer(),
                                                  mesh->getVertexCount() * sizeof(glm::vec3) +
                                                  mesh->getVertexCount() * sizeof(glm::vec2) +
                                                  mesh->getVertexCount() * sizeof(glm::vec4), VK_INDEX_TYPE_UINT32);

                commandBuffer->cmdDrawIndexed(mesh->getIndexCount(), 1, 0, 0, 0);
            }

            commandBuffer->cmdEndRenderPass();
            commandBuffer->stop();
        }
        logger.trace("Successfully created command buffers");
    }

    void Render::createRenderPass() {
        /// Create render pass
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = logicalDevice->surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference{};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = depthImage->getFormat();
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentReference = {};
        depthAttachmentReference.attachment = 1;
        depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = attachments.size();
        renderPassCreateInfo.pAttachments = attachments.data();
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpassDescription;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(logicalDevice->device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
            logger.critical("Failed to create render pass");
        logger.trace("Successfully created render pass");
    }

    void Render::destroyRenderPass() {
        vkDestroyRenderPass(logicalDevice->device, renderPass, nullptr);
        logger.trace("Destroyed render pass");
    }

    void Render::createPipeline() {
        /// Create graphics pipeline
        std::vector<VkPipelineShaderStageCreateInfo> s{};

        s.reserve(shader->getModules().size());
        for (const auto &sh : shader->getModules()) {
            VkPipelineShaderStageCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            createInfo.stage = sh->getStage();
            createInfo.module = sh->getModule();
            createInfo.pName = sh->getEntryPoint().c_str();

            s.push_back(createInfo);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateInfo.vertexBindingDescriptionCount = shader->getBindings().size();
        vertexInputCreateInfo.pVertexBindingDescriptions = shader->getBindings().data();
        vertexInputCreateInfo.vertexAttributeDescriptionCount = shader->getAttributes().size();
        vertexInputCreateInfo.pVertexAttributeDescriptions = shader->getAttributes().data();

        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
        inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) logicalDevice->extent.width;
        viewport.height = (float) logicalDevice->extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        scissor.offset = {0, 0};
        scissor.extent = logicalDevice->extent;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
        viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports = &viewport;
        viewportStateCreateInfo.scissorCount = 1;
        viewportStateCreateInfo.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
        rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth = 1.0f;
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
        rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;

        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_SUBTRACT;

        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f;
        depthStencil.maxDepthBounds = 1.0f;
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {};
        depthStencil.back = {};

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = s.size();
        pipelineCreateInfo.pStages = s.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampling;
        pipelineCreateInfo.pDepthStencilState = &depthStencil;
        pipelineCreateInfo.pColorBlendState = &colorBlending;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        VK_CHECK_RESULT(
                vkCreateGraphicsPipelines(logicalDevice->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                          &pipeline))
        logger.trace("Successfully created a graphics pipeline");
    }

    void Render::destroyPipeline() {
        vkDestroyPipeline(logicalDevice->device, pipeline, nullptr);
        logger.trace("Destroyed pipeline");
    }

    void Render::createPipelineLayout() {
        auto layout = descriptorSetLayout->getDescriptorSetLayout();

        /// Create graphics pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &layout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        VK_CHECK_RESULT(
                vkCreatePipelineLayout(logicalDevice->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout))
        logger.trace("Successfully created pipeline layout");
    }

    void Render::destroyPipelineLayout() {
        vkDestroyPipelineLayout(logicalDevice->device, pipelineLayout, nullptr);
        logger.trace("Destroyed pipeline layout");
    }

    void Render::createUniformBuffers() {
        VkDeviceSize bufferSize = 3 * sizeof(glm::mat4);
        uniformBuffers.reserve(logicalDevice->imageViews.size());
        for (std::vector<VkImage>::size_type i = 0; i < logicalDevice->imageViews.size(); i++)
            uniformBuffers.emplace_back(logicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                        VMA_MEMORY_USAGE_CPU_ONLY); // TODO: This VMA usage flag seems sus
        logger.trace("Successfully created uniform buffers");
    }

    std::vector<std::vector<VkDescriptorSet>> Render::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(scene.entities.size(),
                                                   descriptorSetLayout->getDescriptorSetLayout());

        auto sets = std::vector<std::vector<VkDescriptorSet>>(logicalDevice->imageViews.size());
        for (size_t i = 0; i < sets.size(); i++) {
            sets[i] = descriptorPool->createSets(layouts);
            for (size_t j = 0; j < sets[i].size(); j++) {
                std::vector<VkWriteDescriptorSet> writes{};
                for (const auto &descriptor : shader->getDescriptors()) {
                    VkWriteDescriptorSet write{};
                    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    write.dstSet = sets[i][j];
                    write.dstBinding = descriptor.getBinding();
                    write.dstArrayElement = 0;
                    write.descriptorType = descriptor.getType();
                    write.descriptorCount = 1;

                    switch (descriptor.getType()) {
                        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
                            VkDescriptorBufferInfo buffer{};
                            buffer.buffer = uniformBuffers[i].getBuffer();
                            buffer.offset = 0;
                            buffer.range = descriptor.getSize();

                            write.pBufferInfo = &buffer;
                        }
                        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
                            const auto &texture = scene.entities[j].mesh->getTexture();
                            VkDescriptorImageInfo image{};
                            image.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                            image.imageView = texture != nullptr ? texture->getView() : nullptr;
                            image.sampler = textureSampler->getSampler();

                            write.pImageInfo = &image;
                        }
                        default:
                            break;
                    }

                    writes.insert(writes.begin(), write);
                }
                vkUpdateDescriptorSets(logicalDevice->device, writes.size(), writes.data(), 0, nullptr);
            }
        }
        logger.trace("Successfully updated descriptor sets");

        return sets;
    }

    void Render::create() {
        createDepthImage();
        createSyncObjects();
        descriptorSetLayout = std::make_unique<DescriptorSetLayout>(logicalDevice, *shader);
        createUniformBuffers();
        textureSampler = std::make_unique<ImageSampler>(logicalDevice);
        descriptorPool = std::make_unique<DescriptorPool>(logicalDevice, shader.get(),
                                                          logicalDevice->imageViews.size() * scene.entities.size());
        descriptorSet = createDescriptorSets();
        createRenderPass();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    void Render::destroy() {
        vkDeviceWaitIdle(logicalDevice->device);

        destroyDepthImage();
        destroyFramebuffers();
        destroyRenderPass();
        destroyPipelineLayout();
        destroyPipeline();
        destroySyncObjects();
    }

    /// TODO: Redo this shit, it sucks
    void Render::invalidate() {
        double oldTime = glfwGetTime();
        logger.trace("Invalidating render...");
        logicalDevice->destroySwapchain();
        destroy();
        logicalDevice->createSwapchain();
        logicalDevice->createImageViews();
        create();
        logger.trace("Invalidation took {}ms", glfwGetTime() - oldTime);
    }

    void Render::createDepthImage() {
        const auto &depthImageFormat = physicalDevice->findSupportedFormat(
                {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
                VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

        depthImage = std::make_unique<ImageView>(Image(logicalDevice, logicalDevice->extent.width,
                                                       logicalDevice->extent.height,
                                                       depthImageFormat, VK_IMAGE_TILING_OPTIMAL,
                                                       VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
                                                 VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void Render::destroyDepthImage() {
        depthImage = nullptr;
    }

    double Render::getDeltaTime() const {
        return deltaTime;
    }
}