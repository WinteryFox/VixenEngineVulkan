#include "CommandBuffer.h"

namespace Vixen::Vk {
    CommandBuffer::CommandBuffer(const std::shared_ptr<LogicalDevice> &device) : device(device), fence(device) {
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
        if (!fence.isReady())
            fence.waitAndReset();

        vkFreeCommandBuffers(device->device, device->commandPool, 1, &buffer);
    }

    void
    CommandBuffer::submit(const std::vector<VkSemaphore> &waitSemaphores,
                          const std::vector<VkSemaphore> &signalSemaphores,
                          const std::vector<VkPipelineStageFlags> &masks) {
        if (masks.size() != waitSemaphores.size())
            throw std::runtime_error("Mask count must be equal to semaphore count");
        if (recording)
            stop();
        if (!fence.isReady())
            fence.waitAndReset();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
        submitInfo.pWaitDstStageMask = masks.data();
        submitInfo.signalSemaphoreCount = signalSemaphores.size();
        submitInfo.pSignalSemaphores = signalSemaphores.data();

        fence.submit(device->graphicsQueue, submitInfo);
    }

    void CommandBuffer::wait() {
        fence.wait();
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
        return record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
    }

    CommandBuffer &CommandBuffer::recordSimultaneous() {
        return record(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    }

    CommandBuffer &CommandBuffer::stop() {
        if (!recording) {
            logger.warning("Stop called while not recording");
            return *this;
        }

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

    CommandBuffer &CommandBuffer::cmdBeginRenderPass(const VkRenderPassBeginInfo &info, VkSubpassContents contents) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdBeginRenderPass(buffer, &info, contents);
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdEndRenderPass() {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdEndRenderPass(buffer);
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdBindPipeline(VkPipelineBindPoint point, VkPipeline pipeline) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdBindPipeline(buffer, point, pipeline);
        return *this;
    }

    CommandBuffer &
    CommandBuffer::cmdBindDescriptorSets(VkPipelineBindPoint point, VkPipelineLayout layout, uint32_t firstSet,
                                         const std::vector<VkDescriptorSet> &sets,
                                         const std::vector<uint32_t> &offsets) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdBindDescriptorSets(buffer, point, layout, firstSet, sets.size(), sets.data(), offsets.size(),
                                offsets.data());
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdBindVertexBuffers(uint32_t first, const std::vector<VkBuffer> &buffers,
                                                       const std::vector<VkDeviceSize> &offsets) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");
        if (buffers.size() != offsets.size())
            throw std::runtime_error("Buffers size must be the same as offsets size");

        vkCmdBindVertexBuffers(buffer, first, buffers.size(), buffers.data(), offsets.data());
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdBindIndexBuffer(VkBuffer indexBuffer, VkDeviceSize offset, VkIndexType type) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdBindIndexBuffer(buffer, indexBuffer, offset, type);
        return *this;
    }

    CommandBuffer &CommandBuffer::cmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex,
                                                 int32_t vertexOffset, uint32_t firstInstance) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdDrawIndexed(buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        return *this;
    }

    CommandBuffer &
    CommandBuffer::cmdPushConstants(VkPipelineLayout layout, VkPipelineStageFlags stages, uint32_t offset,
                                    uint32_t size, const void *values) {
        if (!recording)
            throw std::runtime_error("Command buffer is not recording");

        vkCmdPushConstants(buffer, layout, stages, offset, size, values);
        return *this;
    }
}
