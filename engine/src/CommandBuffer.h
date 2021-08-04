#pragma once

#include "Vulkan.h"
#include "LogicalDevice.h"

namespace Vixen {
    class CommandBuffer {
        const std::shared_ptr<LogicalDevice> device;

        VkCommandBuffer buffer{};

    public:
        explicit CommandBuffer(const std::shared_ptr<LogicalDevice> &device);

        ~CommandBuffer();

        void submit();

        void reset();

        void record(VkCommandBufferUsageFlags usage, const std::function<void(VkCommandBuffer)> &f);

        void recordAndSubmit(const std::function<void(VkCommandBuffer)> &f);
    };
}
