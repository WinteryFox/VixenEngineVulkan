#include <memory>
#include <VixenEngine.h>
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include "RenderPass.h"

inline constexpr int VIXEN_TEST_VERSION_MAJOR = 0;
inline constexpr int VIXEN_TEST_VERSION_MINOR = 0;
inline constexpr int VIXEN_TEST_VERSION_PATCH = 1;
inline constexpr const char *VIXEN_EDITOR_NAME = "Vixen Editor";

int main() {
    spdlog::set_level(spdlog::level::trace);
    Vixen::Logger logger = Vixen::Logger(VIXEN_EDITOR_NAME);

    const auto window = std::make_shared<Vixen::Window>(VIXEN_EDITOR_NAME, "../../icon.png");
    const auto instance = std::make_shared<Vixen::Instance>(window, VIXEN_EDITOR_NAME,
                                                            glm::ivec3(VIXEN_TEST_VERSION_MAJOR,
                                                                       VIXEN_TEST_VERSION_MINOR,
                                                                       VIXEN_TEST_VERSION_PATCH));
    const auto physicalDevice = std::make_shared<Vixen::PhysicalDevice>(instance);
    const auto logicalDevice = std::make_shared<Vixen::LogicalDevice>(instance, window, physicalDevice);

    std::unique_ptr<Vixen::Input> input(new Vixen::Input(window));

    const auto meshStore = std::make_unique<Vixen::MeshStore>(logicalDevice, physicalDevice);
    meshStore->loadMesh("../../editor/models/fox/Fox.fbx");
    meshStore->loadMesh("../../editor/models/crystal/Crystal.fbx");
    //meshStore->loadMesh("../../editor/models/michiru/Meshes/MichiruSkel_v001_002.fbx");
    //meshStore->loadMesh("../../editor/models/ruby_rose/Mesh/rubySkel_v001_002.fbx");

    Vixen::Scene scene{};
    scene.camera.position = {0, 0, 3};
    // Fox
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[0], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[1], {}, {}, 0.01f));

    // Crystal
    scene.entities.push_back(Vixen::Entity(meshStore->meshes[2], {}, {}, 1.0f));

    // Michiru
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[4], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[5], {}, {}, 0.01f));

    // Ruby
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[8], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[9], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[10], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[11], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[12], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[13], {}, {}, 0.01f));
    //scene.entities.push_back(Vixen::Entity(meshStore->meshes[14], {}, {}, 0.01f));

    /*
     * framebuffers.reserve(device->imageViews.size());
                                          commandBuffers.reserve(device->imageViews.size());
                                          for (uint32_t i = 0; i < device->imageViews.size(); i++) {
                                              framebuffers.push_back(std::make_shared<Framebuffer>(device, renderPass,
                                                                                                   std::vector<VkImageView>{
                                                                                                           device->imageViews[i],
                                                                                                           depthImage->getView()
                                                                                                   },
                                                                                                   device->extent.width,
                                                                                                   device->extent.height));
                                              commandBuffers.push_back(std::make_shared<CommandBuffer>(device));
                                              auto &commandBuffer = commandBuffers[i];
                                              commandBuffer->recordSimultaneous();

                                              VkRenderPassBeginInfo renderPassBeginInfo = {};
                                              renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                                              renderPassBeginInfo.renderPass = renderPass;
                                              renderPassBeginInfo.framebuffer = framebuffers[i]->getFramebuffer();
                                              renderPassBeginInfo.renderArea.offset = {0, 0};
                                              renderPassBeginInfo.renderArea.extent = device->extent;

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
                                      }
     */

    const auto depthImage = std::make_unique<Vixen::ImageView>(
            Vixen::Image(
                    logicalDevice,
                    logicalDevice->extent.width,
                    logicalDevice->extent.height,
                    physicalDevice->findSupportedFormat(
                            {
                                    VK_FORMAT_D32_SFLOAT,
                                    VK_FORMAT_D32_SFLOAT_S8_UINT,
                                    VK_FORMAT_D24_UNORM_S8_UINT
                            },
                            VK_IMAGE_TILING_OPTIMAL,
                            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                    ),
                    VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
            ),
            VK_IMAGE_ASPECT_DEPTH_BIT
    );

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

    VkSubpassDescription opaquePass{};
    opaquePass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    opaquePass.colorAttachmentCount = 1;
    opaquePass.pColorAttachments = &colorAttachmentReference;
    opaquePass.pDepthStencilAttachment = &depthAttachmentReference;

    auto attachments = std::vector<VkAttachmentDescription>{colorAttachment, depthAttachment};
    auto renderPass = std::make_shared<Vixen::RenderPass>(logicalDevice, std::vector{opaquePass}, attachments);

    auto bufferType = Vixen::BufferType::DOUBLE;
    auto frameBuffers = std::vector<std::shared_ptr<Vixen::Framebuffer>>(static_cast<int>(bufferType));
    auto commandBuffers = std::vector<std::shared_ptr<Vixen::CommandBuffer>>(static_cast<int>(bufferType));
    for (size_t i = 0; i < static_cast<int>(bufferType); i++) {
        auto commandBuffer = std::make_shared<Vixen::CommandBuffer>(logicalDevice);
        commandBuffers[i] = commandBuffer;
        auto frameBuffer = std::make_shared<Vixen::Framebuffer>(
                logicalDevice,
                renderPass,
                logicalDevice->imageViews,
                logicalDevice->extent.width,
                logicalDevice->extent.height
        );
        frameBuffers[i] = frameBuffer;
        commandBuffer->recordSimultaneous([i, scene, logicalDevice, commandBuffer, renderPass, frameBuffers](VkCommandBuffer buffer) {
            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = renderPass->renderPass;
            renderPassBeginInfo.framebuffer = frameBuffers[i]->getFramebuffer();
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
        });
    }

    std::unique_ptr<Vixen::Render> render(new Vixen::Render(
            logicalDevice,
            physicalDevice,
            scene,
            Vixen::Shader::Builder()
                    .addModule(Vixen::ShaderModule::Builder(logicalDevice)
                                       .setShaderStage(VK_SHADER_STAGE_VERTEX_BIT)
                                       .setBytecode("vert.spv")
                                       .build())
                    .addModule(Vixen::ShaderModule::Builder(logicalDevice)
                                       .setShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)
                                       .setBytecode("frag.spv")
                                       .build())
                    .addAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0)
                    .addAttribute(1, 1, VK_FORMAT_R32G32_SFLOAT, 0)
                    .addAttribute(2, 2, VK_FORMAT_R32G32B32A32_SFLOAT, 0)
                    .addBinding(0, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec3))
                    .addBinding(1, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec2))
                    .addBinding(2, VK_VERTEX_INPUT_RATE_VERTEX, sizeof(glm::vec4))
                    .addDescriptor(0, 3 * sizeof(glm::mat4), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                   VK_SHADER_STAGE_VERTEX_BIT)
                    .addDescriptor(1, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                   VK_SHADER_STAGE_FRAGMENT_BIT)
                    .build(),
            commandBuffer));

    std::vector<VkDescriptorPoolSize> sizes = {
            {VK_DESCRIPTOR_TYPE_SAMPLER,                1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       1000},
    };
    auto imGuiPool = Vixen::DescriptorPool(logicalDevice, sizes, 1);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForVulkan(window
                                         ->window, true);
    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = logicalDevice->instance->getInstance();
    info.
            PhysicalDevice = physicalDevice->device;
    info.
            Device = logicalDevice->device;
    info.
            QueueFamily = physicalDevice->graphicsFamilyIndex;
    info.
            Queue = logicalDevice->graphicsQueue;
    info.
            PipelineCache = VK_NULL_HANDLE;
    info.
            DescriptorPool = imGuiPool.getPool();
    info.
            Subpass = 0;
    info.
            MinImageCount = logicalDevice->imageCount;
    info.
            ImageCount = logicalDevice->imageCount;
    info.
            MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    info.
            Allocator = VK_NULL_HANDLE;
    info.
            CheckVkResultFn = Vixen::checkResult;
    ImGui_ImplVulkan_Init(&info, render
            ->renderPass);

    {
        auto buffer = Vixen::CommandBuffer(logicalDevice);
        buffer.

                recordSingleUsage();

        ImGui_ImplVulkan_CreateFontsTexture(buffer
                                                    .buffer);
        buffer.

                stop();

        buffer.

                submit();
        VK_CHECK_RESULT(vkDeviceWaitIdle(logicalDevice->device));

        ImGui_ImplVulkan_DestroyFontUploadObjects();

    }

    int fps = 0;
    double lastTime = 0;
    while (!window->

            shouldClose()

            ) {
        scene.entities[0].rotation.y += 5 * static_cast
                <float>(render
                        ->

                                getDeltaTime()

        );
        window->

                update();

        input->
                update(scene
                               .camera, render->

                getDeltaTime()

        );

        ImGui_ImplVulkan_NewFrame();

        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        ImGui::Begin("Position");
        ImGui::Text("Hello");

        ImGui::End();

        ImGui::Render();

        render->
                render(scene
                               .camera);

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0) {
            logger.info("FPS: {}", fps);
            fps = 0;
            lastTime = currentTime;
        }
    }

    vkDeviceWaitIdle(logicalDevice
                             ->device);

    ImGui_ImplVulkan_Shutdown();

    ImGui_ImplGlfw_Shutdown();

    ImGui::DestroyContext();

    return EXIT_SUCCESS;
}
