#include "Buffer.h"

namespace Vixen {
    Buffer::Buffer(const std::unique_ptr<LogicalDevice> &device, VkDeviceSize size, VkBufferUsageFlags bufferUsage,
                   VmaMemoryUsage allocationUsage)
            : device(device), size(size) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = bufferUsage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = allocationUsage;

        vmaCreateBuffer(device->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation, nullptr);
    }

    Buffer::~Buffer() {
        vmaDestroyBuffer(device->allocator, buffer, allocation);
    }

    template<typename T>
    void Buffer::update(const T *data) {
        if (sizeof(T) > size)
            throw std::runtime_error("Buffer overflow");

        void *d = map();
        memcpy(d, data, sizeof(data));
        unmap();
    }

    void *Buffer::map() {
        void *data;
        vmaMapMemory(device->allocator, allocation, &data);
        return data;
    }

    void Buffer::unmap() {
        vmaUnmapMemory(device->allocator, allocation);
    }

    void Buffer::copy(Buffer &other) {
        auto commandBuffer = beginSingleUseCommandBuffer(device);

        VkBufferCopy copyRegion = {};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, other.getBuffer(), buffer, 1, &copyRegion);

        endSingleUseCommandBuffer(device, commandBuffer);
    }

    VkBuffer Buffer::getBuffer() const {
        return buffer;
    }
}
