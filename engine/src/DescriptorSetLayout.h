#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "LogicalDevice.h"
#include "Shader.h"

namespace Vixen {
    class DescriptorSetLayout {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        VkDescriptorSetLayout layout = VK_NULL_HANDLE;

        static std::vector<VkDescriptorSetLayoutBinding> createBindings(const Shader *shader);

    public:
        DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice,
                            const std::vector<VkDescriptorSetLayoutBinding> &layoutBindings);

        DescriptorSetLayout(const std::unique_ptr<LogicalDevice> &logicalDevice, const Shader *shader);

        ~DescriptorSetLayout();

        [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const;
    };
}
