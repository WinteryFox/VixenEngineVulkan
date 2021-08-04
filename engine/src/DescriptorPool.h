#pragma once

#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"

namespace Vixen {
    class DescriptorPool {
        const std::shared_ptr<LogicalDevice> device;

        VkDescriptorPool pool = VK_NULL_HANDLE;

        static std::vector<VkDescriptorPoolSize>
        createSizes(const std::shared_ptr<LogicalDevice> &logicalDevice, const Shader *shader);

    public:
        DescriptorPool(const std::shared_ptr<LogicalDevice> &logicalDevice,
                       const std::vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets);

        DescriptorPool(const std::shared_ptr<LogicalDevice> &logicalDevice, const Shader *shader, uint32_t maxSets);

        ~DescriptorPool();

        [[nodiscard]] VkDescriptorPool getPool() const;

        [[nodiscard]] const std::shared_ptr<LogicalDevice> &getDevice() const;

        [[nodiscard]] std::vector<VkDescriptorSet> createSets(const std::vector<VkDescriptorSetLayout> &layouts) const;
    };
}
