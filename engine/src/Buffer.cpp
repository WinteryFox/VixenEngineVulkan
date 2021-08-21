#include "Buffer.h"

namespace Vixen {
    Buffer::Buffer(const std::shared_ptr<LogicalDevice> &device, VkDeviceSize size, VkBufferUsageFlags bufferUsage,
                   VmaMemoryUsage allocationUsage)
            : device(device), allocation(nullptr), buffer(nullptr), size(size) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = bufferUsage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = allocationUsage;

        VK_CHECK_RESULT(
                vmaCreateBuffer(device->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation,
                                nullptr))
    }

    Buffer::Buffer(Buffer &&other) noexcept: device(other.device), allocation(std::exchange(other.allocation, nullptr)),
                                             buffer(std::exchange(other.buffer, nullptr)), size(other.size) {}

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

    template<typename T>
    void Buffer::write(const T &data, VkDeviceSize offset) {
        write(&data, sizeof(T), offset);
    }

    void *Buffer::map() {
        void *data = nullptr;
        VK_CHECK_RESULT(vmaMapMemory(device->allocator, allocation, &data))
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
