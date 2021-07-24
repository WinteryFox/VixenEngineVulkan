#ifndef VIXENENGINE_DESCRIPTORPOOL_H
#define VIXENENGINE_DESCRIPTORPOOL_H

#include "LogicalDevice.h"
#include "DescriptorSetLayout.h"

namespace Vixen {
    class DescriptorPool {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        VkDescriptorPool pool = VK_NULL_HANDLE;

    public:
        DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice,
                       const std::vector<VkDescriptorPoolSize> &sizes, int32_t maxSets);

        DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader,
                       uint32_t maxSets);

        ~DescriptorPool();

        static std::vector<VkDescriptorPoolSize>
        createSizes(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader);

        [[nodiscard]] VkDescriptorPool getPool() const;
    };
}

#endif