#include "DescriptorSetLayout.h"

namespace Vixen {
    DescriptorSetLayout::DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                             const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings)
            : logicalDevice(logicalDevice) {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = layoutBindings.size();
        descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();

        auto result = vkCreateDescriptorSetLayout(logicalDevice->device, &descriptorSetLayoutCreateInfo, nullptr,
                                                  &layout);
        if (result != VK_SUCCESS)
            throw VulkanException("Failed to create descriptor set layout", result);
    }

    DescriptorSetLayout::DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice,
                                             const Shader *shader)
            : DescriptorSetLayout(logicalDevice, createBindings(shader)) {}

    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(logicalDevice->device, layout, nullptr);
    }

    std::vector<VkDescriptorSetLayoutBinding> DescriptorSetLayout::createBindings(const Shader *shader) {
        std::vector<VkDescriptorSetLayoutBinding> layoutBindings{};

        for (const auto &shaderBinding : shader->getBindings()) {
            VkDescriptorSetLayoutBinding layoutBinding{};
            layoutBinding.binding = shaderBinding.getInputBinding().binding;
            layoutBinding.descriptorType = shaderBinding.getType();
            layoutBinding.descriptorCount = 1;
            layoutBinding.stageFlags = shaderBinding.getStage();
            layoutBinding.pImmutableSamplers = nullptr;

            layoutBindings.push_back(layoutBinding);
        }

        return layoutBindings;
    }

    VkDescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayout() const {
        return layout;
    }
}