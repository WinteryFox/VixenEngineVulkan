#pragma once

#include <vk_mem_alloc.h>
#include "Vulkan.h"

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

        template<typename T>
        void update(const T *data);

        void* map();

        void unmap();

        void copy(Buffer &other);

        [[nodiscard]] VkBuffer getBuffer() const;
    };
}
