#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "DescriptorPool.h"

namespace Vixen {
    class DescriptorSet {
        const std::shared_ptr<const DescriptorPool> pool;
        std::vector<VkDescriptorSet> set;

    public:
        DescriptorSet(const std::shared_ptr<const DescriptorPool> &pool,
                      const std::vector<VkDescriptorSetLayout> &layouts);

        ~DescriptorSet();

        const std::vector<VkDescriptorSet> &getSet();

        void update();
    };
}
