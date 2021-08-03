#pragma once

#include <vk_mem_alloc.h>
#include <memory>
#include "LogicalDevice.h"
#include "CommandBuffer.h"

namespace Vixen {
    class Buffer {
        const std::unique_ptr<LogicalDevice> &device;

        VmaAllocation allocation = VK_NULL_HANDLE;

        VkBuffer buffer = VK_NULL_HANDLE;

        VkDeviceSize size;

    public:
        Buffer(const std::unique_ptr<LogicalDevice> &device, VkDeviceSize size, VkBufferUsageFlags bufferUsage,
               VmaMemoryUsage allocationUsage);

        Buffer(const Buffer &other) = delete;

        ~Buffer();

        void write(const void *data, VkDeviceSize dataSize, VkDeviceSize offset);

        void *map();

        void unmap();

        void copyFrom(const Buffer &other) const;

        [[nodiscard]] VkBuffer getBuffer() const;
    };
}
