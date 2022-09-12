#include "CommandBuffer.h"

namespace Vixen {
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

    CommandBuffer &CommandBuffer::record(VkCommandBufferUsageFlags usage, const std::function<void(VkCommandBuffer)>& commands) {
        if (recording)
            throw std::runtime_error("Already recording");

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = usage;

        VK_CHECK_RESULT(vkBeginCommandBuffer(buffer, &beginInfo))
        commands(buffer);
        return *this;
    }

    CommandBuffer &CommandBuffer::recordSingleUsage(const std::function<void(VkCommandBuffer)>& commands) {
        return record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, commands);
    }

    CommandBuffer &CommandBuffer::recordSimultaneous(const std::function<void(VkCommandBuffer)>& commands) {
        return record(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, commands);
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

    VkCommandBuffer CommandBuffer::getBuffer() const {
        return buffer;
    }
}
