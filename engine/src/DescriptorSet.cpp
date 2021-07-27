#include "DescriptorSet.h"

namespace Vixen {
    DescriptorSet::DescriptorSet(const std::shared_ptr<const DescriptorPool> &pool,
                                 const std::vector<VkDescriptorSetLayout> &layouts)
            : pool(pool) {
        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = pool->getPool();
        allocateInfo.descriptorSetCount = layouts.size();
        allocateInfo.pSetLayouts = layouts.data();

        const auto result = vkAllocateDescriptorSets(pool->getDevice()->device, &allocateInfo, set.data());
        if (result != VK_SUCCESS)
            throw VulkanException("Failed to allocate descriptor sets", result);
        trace("Created descriptor set");
    }

    DescriptorSet::~DescriptorSet() {
        vkFreeDescriptorSets(pool->getDevice()->device, pool->getPool(), set.size(), set.data());
        trace("Freed descriptor set");
    }

    const std::vector<VkDescriptorSet> &DescriptorSet::getSet() {
        return set;
    }
}