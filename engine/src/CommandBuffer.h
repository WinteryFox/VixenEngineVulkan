#pragma once

#include "Vulkan.h"
#include "LogicalDevice.h"
#include "Fence.h"

namespace Vixen {
    class CommandBuffer {
        const Logger logger{"CommandBuffer"};

        const std::shared_ptr<LogicalDevice> device;

        VkCommandBuffer buffer{};

        bool recording = false;

        Fence fence;

        CommandBuffer &record(VkCommandBufferUsageFlags usage);

    public:
        explicit CommandBuffer(const std::shared_ptr<LogicalDevice> &device);

        CommandBuffer(const CommandBuffer &) = delete;

        CommandBuffer &operator=(const CommandBuffer &) = delete;

        ~CommandBuffer();

        void
        submit(const std::vector<VkSemaphore> &waitSemaphores = {},
               const std::vector<VkSemaphore> &signalSemaphores = {},
               const std::vector<VkPipelineStageFlags> &masks = {});

        void wait();

        CommandBuffer &recordSingleUsage();

        CommandBuffer &recordSimultaneous();

        CommandBuffer &stop();

        CommandBuffer &cmdCopyBuffer(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy> &regions);

        CommandBuffer &cmdCopyBufferToImage(VkBuffer source, VkImage destination, VkImageLayout layout,
                                            const std::vector<VkBufferImageCopy> &regions);

        CommandBuffer &cmdPipelineBarrier(VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages,
                                          VkDependencyFlags dependencies, const std::vector<VkMemoryBarrier> &barriers,
                                          const std::vector<VkBufferMemoryBarrier> &bufferBarriers,
                                          const std::vector<VkImageMemoryBarrier> &imageBarriers);

        CommandBuffer &cmdBeginRenderPass(const VkRenderPassBeginInfo &info, VkSubpassContents contents);

        CommandBuffer &cmdEndRenderPass();

        CommandBuffer &cmdBindPipeline(VkPipelineBindPoint point, VkPipeline pipeline);

        CommandBuffer &cmdBindDescriptorSets(VkPipelineBindPoint point, VkPipelineLayout layout, uint32_t firstSet,
                                             const std::vector<VkDescriptorSet> &sets,
                                             const std::vector<uint32_t> &offsets);

        CommandBuffer &cmdBindVertexBuffers(uint32_t first, const std::vector<VkBuffer> &buffers,
                                            const std::vector<VkDeviceSize> &offsets);

        CommandBuffer &cmdBindIndexBuffer(VkBuffer indexBuffer, VkDeviceSize offset, VkIndexType type);

        CommandBuffer &
        cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset,
                       uint32_t firstInstance);
    };
}
