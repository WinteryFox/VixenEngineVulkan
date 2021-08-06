#include "Buffer.h"

namespace Vixen {
    Buffer::Buffer(const std::shared_ptr<LogicalDevice> &device, VkDeviceSize size, VkBufferUsageFlags bufferUsage,
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

    void Buffer::write(const void *data, const VkDeviceSize dataSize, VkDeviceSize offset) {
        if (offset + dataSize > size)
            throw std::runtime_error("Buffer overflow");

        void *d = map();
        memcpy(d, data, dataSize);
        unmap();
    }

    void *Buffer::map() {
        void *data = nullptr;
        vmaMapMemory(device->allocator, allocation, &data);
        return data;
    }

    void Buffer::unmap() {
        vmaUnmapMemory(device->allocator, allocation);
    }

    void Buffer::copyFrom(const Buffer &other) const {
        VkBufferCopy copyRegion = {};
        copyRegion.size = size;

        CommandBuffer(device)
                .recordSingleUsage()
                .cmdCopyBuffer(other.getBuffer(), buffer, std::vector<VkBufferCopy>{copyRegion})
                .submit();
    }

    VkBuffer Buffer::getBuffer() const {
        return buffer;
    }
}
