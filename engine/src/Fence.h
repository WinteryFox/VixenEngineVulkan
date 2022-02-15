#pragma once

#include "LogicalDevice.h"

namespace Vixen {
    class Fence {
        const std::shared_ptr<LogicalDevice> device;

        VkFence fence{};

    public:
        explicit Fence(std::shared_ptr<LogicalDevice> device);

        Fence(const Fence &) = delete;

        Fence &operator=(const Fence &) = delete;

        ~Fence();

        bool isReady();

        void submit(VkQueue queue, const VkSubmitInfo &info);

        Fence &wait();

        Fence &waitAndReset();

        Fence &reset();
    };
}
