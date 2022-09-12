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

        CommandBuffer &record(VkCommandBufferUsageFlags usage, const std::function<void(VkCommandBuffer)>& commands);

        CommandBuffer &recordSingleUsage(const std::function<void(VkCommandBuffer)>& commands);

        CommandBuffer &recordSimultaneous(const std::function<void(VkCommandBuffer)>& commands);

        CommandBuffer &stop();

        VkCommandBuffer getBuffer() const;
    };
}
