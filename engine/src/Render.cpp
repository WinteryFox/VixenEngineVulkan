#include "Render.h"

namespace Vixen {
    Render::Render(const std::unique_ptr<LogicalDevice> &device, const std::unique_ptr<PhysicalDevice> &physicalDevice,
                   const Scene &scene, const std::unique_ptr<VertexShader> &vertex,
                   const std::unique_ptr<FragmentShader> &fragment, const int framesInFlight)
            : logicalDevice(device), physicalDevice(physicalDevice), scene(scene), vertex(vertex),
              fragment(fragment), framesInFlight(framesInFlight) {
        create();
    }

    Render::~Render() {
        destroy();
    }

    void Render::render(const std::unique_ptr<Camera> &camera) {
        vkWaitForFences(logicalDevice->device, 1, &fences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(logicalDevice->device, logicalDevice->swapchain,
                                                std::numeric_limits<uint64_t>::max(),
                                                imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        vertex->mvp.view = camera->getView();
        vertex->mvp.projection = camera->getProjection(
                (float) logicalDevice->extent.width / (float) logicalDevice->extent.height);
        vertex->mvp.projection[1][1] *= -1.0f;
        updateUniformBuffer(scene.entities[0], imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            invalidate();
            return;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            fatal("Failed to acquire image " + std::to_string(result));
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(logicalDevice->device, 1, &fences[currentFrame]);

        if (vkQueueSubmit(logicalDevice->graphicsQueue, 1, &submitInfo, fences[currentFrame]) != VK_SUCCESS)
            fatal("Failed to submit command to queue");

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {logicalDevice->swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        vkQueuePresentKHR(logicalDevice->presentQueue, &presentInfo);

        currentFrame = (currentFrame + 1) % framesInFlight;
    }

    void Render::updateUniformBuffer(Entity entity, uint32_t imageIndex) {
        vertex->mvp.model = entity.getModelMatrix();

        void *data;
        vmaMapMemory(logicalDevice->allocator, uniformBuffersMemory[imageIndex], &data);
        memcpy(data, &vertex->mvp, sizeof(vertex->mvp));
        vmaUnmapMemory(logicalDevice->allocator, uniformBuffersMemory[imageIndex]);
    }

    void Render::createFramebuffers() {
        framebuffers.resize(logicalDevice->imageViews.size());
        for (std::vector<VkImageView>::size_type i = 0; i < logicalDevice->imageViews.size(); i++) {
            VkImageView attachments[] = {logicalDevice->imageViews[i]};

            VkFramebufferCreateInfo framebufferCreateInfo = {};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = renderPass;
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = logicalDevice->extent.width;
            framebufferCreateInfo.height = logicalDevice->extent.height;
            framebufferCreateInfo.layers = 1;

            if (vkCreateFramebuffer(logicalDevice->device, &framebufferCreateInfo, nullptr, &framebuffers[i]) !=
                VK_SUCCESS)
                fatal("Failed to create a frame buffer");
        }
        trace("Successfully created frame buffers");
    }

    void Render::destroyFramebuffers() {
        for (auto &framebuffer : framebuffers)
            vkDestroyFramebuffer(logicalDevice->device, framebuffer, nullptr);
        trace("Destroyed framebuffers");
    }

    void Render::createSyncObjects() {
        /// Create fences and semaphores
        imageAvailableSemaphores.resize(framesInFlight);
        renderFinishedSemaphores.resize(framesInFlight);
        fences.resize(framesInFlight);

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < framesInFlight; i++)
            if (vkCreateSemaphore(logicalDevice->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateSemaphore(logicalDevice->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) !=
                VK_SUCCESS ||
                vkCreateFence(logicalDevice->device, &fenceCreateInfo, nullptr, &fences[i]) != VK_SUCCESS)
                fatal("Failed to create semaphores and fences");
        trace("Successfully created semaphores and fences");
    }

    void Render::destroySyncObjects() {
        for (size_t i = 0; i < framesInFlight; i++) {
            vkDestroySemaphore(logicalDevice->device, imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice->device, renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice->device, fences[i], nullptr);
        }
        trace("Destroyed sync objects");
    }

    void Render::createCommandBuffers() {
        commandBuffers.resize(framebuffers.size());
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS)
            fatal("Failed to allocate command buffers");
        trace("Successfully allocated command buffers");

        for (std::vector<VkCommandBuffer>::size_type i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            beginInfo.pInheritanceInfo = nullptr;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
                fatal("Failed to begin command buffer");

            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderPass;
            renderPassBeginInfo.framebuffer = framebuffers[i];
            renderPassBeginInfo.renderArea.offset = {0, 0};
            renderPassBeginInfo.renderArea.extent = logicalDevice->extent;

            VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 0.0f};
            renderPassBeginInfo.clearValueCount = 1;
            renderPassBeginInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            for (const auto &entity : scene.entities) {
                const auto &mesh = entity.mesh;
                /// Bind the mesh's buffers
                std::vector<VkBuffer> buffers{mesh->buffer};
                std::vector<VkDeviceSize> offsets{0};
                vkCmdBindVertexBuffers(commandBuffers[i], 0, buffers.size(), buffers.data(), offsets.data());
                vkCmdBindIndexBuffer(commandBuffers[i], mesh->buffer, sizeof(glm::vec3) * mesh->vertexCount,
                                     VK_INDEX_TYPE_UINT32);

                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                                        &descriptorSets[i], 0, nullptr);

                /// Draw the mesh
                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(mesh->indexCount), 1, 0, 0, 0);
            }

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
                fatal("Failed to reset command buffer");
        }
        trace("Successfully created command buffers");
    }

    void Render::destroyCommandBuffers() {
        vkFreeCommandBuffers(logicalDevice->device, commandPool, commandBuffers.size(), commandBuffers.data());
        trace("Freed command buffers");
    }

    void Render::createRenderPass() {
        /// Create render pass
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = logicalDevice->surfaceFormat.format;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;

        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassCreateInfo = {};
        renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassCreateInfo.attachmentCount = 1;
        renderPassCreateInfo.pAttachments = &colorAttachment;
        renderPassCreateInfo.subpassCount = 1;
        renderPassCreateInfo.pSubpasses = &subpass;
        renderPassCreateInfo.dependencyCount = 1;
        renderPassCreateInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(logicalDevice->device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
            fatal("Failed to create render pass");
        trace("Successfully created render pass");
    }

    void Render::destroyRenderPass() {
        vkDestroyRenderPass(logicalDevice->device, renderPass, nullptr);
        trace("Destroyed render pass");
    }

    void Render::createPipeline() {
        /// Create graphics pipeline
        VkPipelineShaderStageCreateInfo vertCreateInfo = {};
        vertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertCreateInfo.module = vertex->shader;
        vertCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragCreateInfo = {};
        fragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragCreateInfo.module = fragment->shader;
        fragCreateInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaders[] = {vertCreateInfo, fragCreateInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateInfo.vertexBindingDescriptionCount = scene.entities.size();
        vertexInputCreateInfo.pVertexBindingDescriptions = &scene.entities[0].mesh->bindingDescription;
        vertexInputCreateInfo.vertexAttributeDescriptionCount = scene.entities.size();
        vertexInputCreateInfo.pVertexAttributeDescriptions = &scene.entities[0].mesh->attributeDescription;

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

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = 2;
        pipelineCreateInfo.pStages = shaders;
        pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
        pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisampling;
        pipelineCreateInfo.pDepthStencilState = nullptr;
        pipelineCreateInfo.pColorBlendState = &colorBlending;
        pipelineCreateInfo.pDynamicState = nullptr;
        pipelineCreateInfo.layout = pipelineLayout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(logicalDevice->device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                      &pipeline) !=
            VK_SUCCESS)
            fatal("Failed to create graphics pipeline");
        trace("Successfully created a graphics pipeline");
    }

    void Render::destroyPipeline() {
        vkDestroyPipeline(logicalDevice->device, pipeline, nullptr);
        trace("Destroyed pipeline");
    }

    void Render::createPipelineLayout() {
        /// Create graphics pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(logicalDevice->device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS)
            fatal("Failed to create pipeline layout");
        trace("Successfully created pipeline layout");
    }

    void Render::destroyPipelineLayout() {
        vkDestroyPipelineLayout(logicalDevice->device, pipelineLayout, nullptr);
        trace("Destroyed pipeline layout");
    }

    void Render::createCommandPool() {
        /// Create command pool
        VkCommandPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolCreateInfo.queueFamilyIndex = physicalDevice->graphicsFamilyIndex;
        poolCreateInfo.flags = 0;

        if (vkCreateCommandPool(logicalDevice->device, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
            fatal("Failed to create command pool");
        trace("Successfully created command pool");
    }

    void Render::destroyCommandPool() {
        vkDestroyCommandPool(logicalDevice->device, commandPool, nullptr);
        trace("Destroyed command pool");
    }

    void Render::createDescriptorSetLayout() {
        VkDescriptorSetLayoutBinding layoutBinding = {};
        layoutBinding.binding = 0;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.descriptorCount = 1;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        layoutBinding.pImmutableSamplers = nullptr;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &layoutBinding;

        if (vkCreateDescriptorSetLayout(logicalDevice->device, &descriptorSetLayoutCreateInfo, nullptr,
                                        &descriptorSetLayout) != VK_SUCCESS)
            fatal("Failed to create descriptor set layout");
        trace("Successfully created descriptor set layout");
    }

    void Render::destroyDescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(logicalDevice->device, descriptorSetLayout, nullptr);
        trace("Destroyed descriptor set layout");
    }

    void Render::createUniformBuffers() {
        VkDeviceSize bufferSize = sizeof(VertexShader::ModelViewProjection);
        uniformBuffers.resize(logicalDevice->images.size());
        uniformBuffersMemory.resize(logicalDevice->images.size());

        for (std::vector<VkImage>::size_type i = 0; i < logicalDevice->images.size(); i++)
            createBuffer(logicalDevice, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_ONLY,
                         uniformBuffers[i], uniformBuffersMemory[i]);
        trace("Successfully created uniform buffers");
    }

    void Render::destroyUniformBuffers() {
        for (std::vector<VkBuffer>::size_type i = 0; i < uniformBuffers.size(); i++)
            vmaDestroyBuffer(logicalDevice->allocator, uniformBuffers[i], uniformBuffersMemory[i]);
        trace("Destroyed uniform buffers");
    }

    VkDescriptorPool Render::createDescriptorPool() {
        VkDescriptorPool pool;

        VkDescriptorPoolSize descriptorPoolSize{};
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize.descriptorCount = static_cast<uint32_t>(logicalDevice->images.size());

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
        descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
        descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(logicalDevice->images.size());

        if (vkCreateDescriptorPool(logicalDevice->device, &descriptorPoolCreateInfo, nullptr, &pool) != VK_SUCCESS)
            fatal("Failed to create descriptor pool");
        trace("Successfully created descriptor pool");

        return pool;
    }

    void Render::destroyDescriptorPool() {
        vkDestroyDescriptorPool(logicalDevice->device, descriptorPool, nullptr);
        trace("Destroyed descriptor pool");
    }

    std::vector<VkDescriptorSet> Render::createDescriptorSets() {
        std::vector<VkDescriptorSet> sets;
        std::vector<VkDescriptorSetLayout> layouts(logicalDevice->images.size(), descriptorSetLayout);

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool = descriptorPool;
        descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(logicalDevice->images.size());
        descriptorSetAllocateInfo.pSetLayouts = layouts.data();

        sets.resize(logicalDevice->images.size());
        if (vkAllocateDescriptorSets(logicalDevice->device, &descriptorSetAllocateInfo, sets.data()) != VK_SUCCESS)
            fatal("Failed to allocate descriptor sets");
        trace("Successfully allocated descriptor sets");

        for (std::vector<VkImage>::size_type i = 0; i < logicalDevice->images.size(); i++) {
            VkDescriptorBufferInfo descriptorBufferInfo{};
            descriptorBufferInfo.buffer = uniformBuffers[i];
            descriptorBufferInfo.offset = 0;
            descriptorBufferInfo.range = sizeof(VertexShader::ModelViewProjection);

            VkWriteDescriptorSet writeDescriptorSet{};
            writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptorSet.dstSet = sets[i];
            writeDescriptorSet.dstBinding = 0;
            writeDescriptorSet.dstArrayElement = 0;
            writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            writeDescriptorSet.descriptorCount = 1;
            writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
            writeDescriptorSet.pImageInfo = nullptr;
            writeDescriptorSet.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(logicalDevice->device, 1, &writeDescriptorSet, 0, nullptr);
        }
        trace("Successfully updated descriptor sets");

        return sets;
    }

    void Render::create() {
        createSyncObjects();
        createDescriptorSetLayout();
        createUniformBuffers();
        descriptorPool = createDescriptorPool();
        descriptorSets = createDescriptorSets();
        createRenderPass();
        createFramebuffers();
        createCommandPool();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    void Render::destroy() {
        vkDeviceWaitIdle(logicalDevice->device);

        destroyUniformBuffers();
        destroyDescriptorSetLayout();
        destroyFramebuffers();
        destroyCommandBuffers();
        destroyCommandPool();
        destroyRenderPass();
        destroyPipelineLayout();
        destroyPipeline();
        destroySyncObjects();
        destroyDescriptorPool();
    }

    /// TODO: Redo this shit, it sucks
    void Render::invalidate() {
        double oldTime = glfwGetTime();
        trace("Invalidating render...");
        logicalDevice->destroySwapchain();
        destroy();
        logicalDevice->createSwapchain();
        logicalDevice->createImageViews();
        create();
        trace("Invalidation took " + std::to_string(glfwGetTime() - oldTime));
    }
}