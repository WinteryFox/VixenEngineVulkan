#include "CommandBuffer.h"

namespace Vixen {
    CommandBuffer::CommandBuffer(const std::shared_ptr<LogicalDevice> &device) : device(device) {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = device->commandPool;
        allocateInfo.commandBufferCount = 1;

        vkAllocateCommandBuffers(device->device, &allocateInfo, &buffer);
    }

    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(device->device, device->commandPool, 1, &buffer);
    }

    void CommandBuffer::submit() {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;

        vkQueueSubmit(device->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(device->graphicsQueue);
    }

    void CommandBuffer::reset() {
        vkResetCommandBuffer(buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
    }

    void CommandBuffer::record(VkCommandBufferUsageFlags usage, const std::function<void(VkCommandBuffer)> &f) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = usage;

        vkBeginCommandBuffer(buffer, &beginInfo);
        f(buffer);
        vkEndCommandBuffer(buffer);
    }

    void CommandBuffer::recordAndSubmit(const std::function<void(VkCommandBuffer)> &f) {
        record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, f);
        submit();
    }
}
