#include "DescriptorPool.h"

namespace Vixen::Vk {
    DescriptorPool::DescriptorPool(const std::shared_ptr<LogicalDevice> &logicalDevice,
                                   const std::vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets)
            : device(logicalDevice) {
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.poolSizeCount = sizes.size();
        info.pPoolSizes = sizes.data();
        info.maxSets = maxSets;

        VK_CHECK_RESULT(vkCreateDescriptorPool(logicalDevice->device, &info, nullptr, &pool))
    }

    DescriptorPool::DescriptorPool(const std::shared_ptr<LogicalDevice> &logicalDevice,
                                   const Shader *shader, uint32_t maxSets)
            : DescriptorPool(logicalDevice, createSizes(logicalDevice, shader), maxSets) {}

    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(device->device, pool, nullptr);
    }

    VkDescriptorPool DescriptorPool::getPool() const {
        return pool;
    }

    const std::shared_ptr<LogicalDevice> &DescriptorPool::getDevice() const {
        return device;
    }

    std::vector<VkDescriptorPoolSize>
    DescriptorPool::createSizes(const std::shared_ptr<LogicalDevice> &logicalDevice, const Shader *shader) {
        std::vector<VkDescriptorPoolSize> sizes{};
        sizes.reserve(shader->getDescriptors().size());

        for (const auto &descriptor : shader->getDescriptors()) {
            VkDescriptorPoolSize size{};
            size.type = descriptor.getType();
            size.descriptorCount = 1;

            sizes.push_back(size);
        }

        return sizes;
    }

    std::vector<VkDescriptorSet> DescriptorPool::createSets(const std::vector<VkDescriptorSetLayout> &layouts) const {
        VkDescriptorSetAllocateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        info.descriptorPool = pool;
        info.descriptorSetCount = layouts.size();
        info.pSetLayouts = layouts.data();

        std::vector<VkDescriptorSet> sets(layouts.size());
        VK_CHECK_RESULT(vkAllocateDescriptorSets(device->device, &info, sets.data()))
        return sets;
    }
}