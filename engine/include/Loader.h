#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <memory>
#include "LogicalDevice.h"

namespace Vixen {
    /**
     * Create and allocate a new buffer, it is up to the user to delete the buffer and free the device memory
     *
     * @param[in] logicalDevice The logical device to create this buffer on
     * @param[in] size The size to allocate
     * @param[in] usage How this buffer will be used
     * @param[out] buffer The buffer to write to
     * @param[out] allocation The buffer memory allocated to by VMA
     * @return Returns true on successful creation and allocation of the buffer
     * @see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferUsageFlagBits.html
     */
    static bool
    createBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
                 VmaMemoryUsage vmaUsage, VkBuffer &buffer, VmaAllocation &allocation) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = vmaUsage;

        if (vmaCreateBuffer(logicalDevice->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation,
                            nullptr) != VK_SUCCESS) {
            error("Failed to allocate buffer memory");
            return false;
        }

        return true;
    }

    /**
     * Copy the data from one buffer to another
     *
     * @param[in] logicalDevice The device to use for the copy
     * @param[in] src The source buffer
     * @param[out] dst The destination buffer
     * @param[in] size The size of the source buffer
     */
    static void copyBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &src, VkBuffer &dst,
                           VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = logicalDevice->transferCommandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkResetFences(logicalDevice->device, 1, &logicalDevice->transferFence);

        vkQueueSubmit(logicalDevice->transferQueue, 1, &submitInfo, logicalDevice->transferFence);
        vkWaitForFences(logicalDevice->device, 1, &logicalDevice->transferFence, true,
                        std::numeric_limits<std::uint64_t>::max());

        vkFreeCommandBuffers(logicalDevice->device, logicalDevice->transferCommandPool, 1, &commandBuffer);
    }

    /**
     * Creates and allocates an index buffer for a mesh
     *
     * @param logicalDevice The device to create the mesh for
     * @param vertices The vertices of the mesh
     * @param indices The indices of the mesh
     * @param buffer The index buffer to write to
     * @param allocation The VMA allocation
     * @return Returns the buffer and allocation for the mesh
     */
    static bool
    createMeshBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<glm::vec3> &vertices,
                     const std::vector<uint32_t> &indices, VkBuffer &buffer, VmaAllocation &allocation) {
        VkDeviceSize vertexBufferSize = sizeof(glm::vec3) * vertices.size();
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;
        createBuffer(logicalDevice, vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VMA_MEMORY_USAGE_CPU_TO_GPU,
                     stagingBuffer, stagingAllocation);

        void *data;
        vmaMapMemory(logicalDevice->allocator, stagingAllocation, &data);
        memcpy(data, vertices.data(), (size_t) vertexBufferSize);
        memcpy(static_cast<char *>(data) + static_cast<size_t>(vertexBufferSize), indices.data(),
               (size_t) indexBufferSize);
        vmaUnmapMemory(logicalDevice->allocator, stagingAllocation);

        if (!createBuffer(logicalDevice, vertexBufferSize + indexBufferSize,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, buffer, allocation)) {
            error("Failed to create buffer for vertices");
            return false;
        }

        copyBuffer(logicalDevice, stagingBuffer, buffer, vertexBufferSize + indexBufferSize);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);

        return true;
    }
}