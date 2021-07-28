#pragma once

#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"

namespace Vixen {
    class DescriptorPool {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        VkDescriptorPool pool = VK_NULL_HANDLE;

    public:
        DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice,
                       const std::vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets);

        DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader, uint32_t maxSets);

        ~DescriptorPool();

        [[nodiscard]] VkDescriptorPool getPool() const;

        [[nodiscard]] const std::unique_ptr<LogicalDevice> &getDevice() const;

        static std::vector<VkDescriptorPoolSize>
        createSizes(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader);
    };
}
