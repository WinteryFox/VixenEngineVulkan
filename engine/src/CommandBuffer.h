#pragma once

#include "Vulkan.h"
#include "LogicalDevice.h"

namespace Vixen {
    class CommandBuffer {
        const Logger logger{"CommandBuffer"};

        const std::shared_ptr<LogicalDevice> device;

        VkCommandBuffer buffer{};

        bool recording = false;

    public:
        explicit CommandBuffer(const std::shared_ptr<LogicalDevice> &device);

        CommandBuffer(const CommandBuffer &) = delete;

        CommandBuffer &operator=(const CommandBuffer &) = delete;

        ~CommandBuffer();

        void submit();

        void submitAndWait();

        void reset();

        CommandBuffer &record(VkCommandBufferUsageFlags usage);

        CommandBuffer &stop();

        CommandBuffer &cmdCopyBuffer(VkBuffer source, VkBuffer destination, const std::vector<VkBufferCopy> &regions);

        CommandBuffer &cmdCopyBufferToImage(VkBuffer source, VkImage destination, VkImageLayout layout,
                                                    const std::vector<VkBufferImageCopy> &regions);

        CommandBuffer &cmdPipelineBarrier(VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages,
                                                  VkDependencyFlags dependencies, const std::vector<VkMemoryBarrier> &barriers,
                                                  const std::vector<VkBufferMemoryBarrier> &bufferBarriers,
                                                  const std::vector<VkImageMemoryBarrier> &imageBarriers);
    };
}
