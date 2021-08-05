#include "CommandBuffer.h"

namespace Vixen {
    CommandBuffer::CommandBuffer(const std::shared_ptr<LogicalDevice> &device) : device(device) {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = device->commandPool;
        allocateInfo.commandBufferCount = 1;

        VK_CHECK_RESULT(vkAllocateCommandBuffers(device->device, &allocateInfo, &buffer))
    }

    CommandBuffer::~CommandBuffer() {
        if (recording) {
            stop();
            logger.warning("Command buffer was still recording when destructor was called");
        }

        vkFreeCommandBuffers(device->device, device->commandPool, 1, &buffer);
    }

    void CommandBuffer::submit() {
        stop();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;

        VK_CHECK_RESULT(vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE))
        VK_CHECK_RESULT(vkQueueWaitIdle(device->graphicsQueue))
    }

    void CommandBuffer::submitAndWait() {
        submit();
        VK_CHECK_RESULT(vkQueueWaitIdle(device->graphicsQueue))
    }

    void CommandBuffer::reset() {
        if (recording)
            throw std::runtime_error("Cannot reset command buffer while recording");

        VK_CHECK_RESULT(vkResetCommandBuffer(buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT))
    }

    CommandBuffer &CommandBuffer::record(VkCommandBufferUsageFlags usage) {
        if (recording)
            throw std::runtime_error("Already recording");

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = usage;

        VK_CHECK_RESULT(vkBeginCommandBuffer(buffer, &beginInfo))
        recording = true;
        return *this;
    }

    CommandBuffer &CommandBuffer::recordSingleUsage() {
        record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        return *this;
    }

    CommandBuffer &CommandBuffer::stop() {
        VK_CHECK_RESULT(vkEndCommandBuffer(buffer))
        recording = false;
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdCopyBuffer(VkBuffer source, VkBuffer destination,
                                                const std::vector<VkBufferCopy> &regions) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdCopyBuffer(buffer, source, destination, regions.size(), regions.data());
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdCopyBufferToImage(VkBuffer source, VkImage destination, VkImageLayout layout,
                                                       const std::vector<VkBufferImageCopy> &regions) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdCopyBufferToImage(buffer, source, destination, layout, regions.size(), regions.data());
        return *this;
    }

    CommandBuffer &
    CommandBuffer::cmdPipelineBarrier(VkPipelineStageFlags sourceStages, VkPipelineStageFlags destinationStages,
                                      VkDependencyFlags dependencies, const std::vector<VkMemoryBarrier> &barriers,
                                      const std::vector<VkBufferMemoryBarrier> &bufferBarriers,
                                      const std::vector<VkImageMemoryBarrier> &imageBarriers) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdPipelineBarrier(buffer, sourceStages, destinationStages, dependencies, barriers.size(), barriers.data(),
                             bufferBarriers.size(), bufferBarriers.data(), imageBarriers.size(), imageBarriers.data());
        return *this;
    }
}
