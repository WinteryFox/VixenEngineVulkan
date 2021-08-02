#include "DescriptorSetLayout.h"

namespace Vixen {
    DescriptorSetLayout::DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                             const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings)
            : logicalDevice(logicalDevice) {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = layoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

        VK_CHECK_RESULT(
                vkCreateDescriptorSetLayout(logicalDevice->device, &descriptorSetLayoutCreateInfo, nullptr, &layout))
    }

    DescriptorSetLayout::DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                             const Shader &shader)
            : DescriptorSetLayout(logicalDevice, createBindings(shader)) {}

    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(logicalDevice->device, layout, nullptr);
    }

    std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayout::createBindings(const Shader &shader) {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

        for (const auto &descriptor : shader.getDescriptors()) {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = descriptor.getBinding();
            layoutBinding.descriptorType = descriptor.getType();
            layoutBinding.descriptorCount = 1;
            layoutBinding.stageFlags = descriptor.getStage();
            layoutBinding.pImmutableSamplers = nullptr;

            layoutBindings.push_back(layoutBinding);
        }

        return layoutBindings;
    }

    VkDescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayout() const {
        return layout;
    }
}