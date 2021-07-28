#include "DescriptorPool.h"

namespace Vixen {
    DescriptorPool::DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                   const std::vector<VkDescriptorPoolSize> &sizes, uint32_t maxSets)
            : logicalDevice(logicalDevice) {
        VkDescriptorPoolCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        info.poolSizeCount = sizes.size();
        info.pPoolSizes = sizes.data();
        info.maxSets = maxSets;
        info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        vkCreateDescriptorPool(logicalDevice->device, &info, nullptr, &pool);
    }

    DescriptorPool::DescriptorPool(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                   const Shader *shader, uint32_t maxSets)
            : DescriptorPool(logicalDevice, createSizes(logicalDevice, shader), maxSets) {}

    DescriptorPool::~DescriptorPool() {
        vkDestroyDescriptorPool(logicalDevice->device, pool, nullptr);
    }

    VkDescriptorPool DescriptorPool::getPool() const {
        return pool;
    }

    const std::unique_ptr<LogicalDevice> &DescriptorPool::getDevice() const {
        return logicalDevice;
    }

    std::vector<VkDescriptorPoolSize>
    DescriptorPool::createSizes(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader) {
        std::vector<VkDescriptorPoolSize> sizes{};

        for (const auto &binding : shader->getBindings()) {
            VkDescriptorPoolSize size{};
            size.type = binding.getType();
            size.descriptorCount = logicalDevice->images.size();

            sizes.push_back(size);
        }

        return sizes;
    }
}