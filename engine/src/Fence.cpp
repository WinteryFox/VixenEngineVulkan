#include "Fence.h"

namespace Vixen {
    Fence::Fence(std::shared_ptr<LogicalDevice> device) : device(device) {
        VkFenceCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_CHECK_RESULT(vkCreateFence(device->device, &info, nullptr, &fence))
    }

    Fence::~Fence() {
        vkDestroyFence(device->device, fence, nullptr);
    }

    bool Fence::isReady() {
        auto result = vkGetFenceStatus(device->device, fence);
        if (result == VK_SUCCESS) return true;
        if (result != VK_NOT_READY) VK_CHECK_RESULT(result)

        return false;
    }

    void Fence::submit(VkQueue queue, const VkSubmitInfo &info) {
        reset();
        vkQueueSubmit(queue, 1, &info, fence);
    }

    Fence& Fence::wait() {
        if (isReady())
            return *this;

        vkWaitForFences(device->device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max());
        return *this;
    }

    Fence& Fence::waitAndReset() {
        return wait().reset();
    }

    Fence& Fence::reset() {
        if (!isReady())
            return *this;

        vkResetFences(device->device, 1, &fence);
        return *this;
    }
}
