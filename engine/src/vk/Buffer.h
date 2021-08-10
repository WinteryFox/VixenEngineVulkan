#pragma once

#include <vk_mem_alloc.h>
#include <memory>
#include "LogicalDevice.h"
#include "CommandBuffer.h"

namespace Vixen::Vk {
    class Buffer {
        const std::shared_ptr<LogicalDevice> device;

        VmaAllocation allocation;

        VkBuffer buffer;

        VkDeviceSize size;

    public:
        Buffer(const std::shared_ptr<LogicalDevice> &device, VkDeviceSize size, VkBufferUsageFlags bufferUsage,
               VmaMemoryUsage allocationUsage);

        Buffer(const Buffer &) = delete;

        Buffer(Buffer&& other) noexcept;

        ~Buffer();

        void write(const void *data, VkDeviceSize dataSize, VkDeviceSize offset);

        template<typename T>
        void write(const T &data, VkDeviceSize offset);

        void *map();

        void unmap();

        void copyFrom(const Buffer &other) const;

        [[nodiscard]] VkBuffer getBuffer() const;
    };
}
