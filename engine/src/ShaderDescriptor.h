#pragma once

#include <cstdio>
#include <vulkan/vulkan.h>

namespace Vixen {
    class ShaderDescriptor {
        const uint32_t binding;
        const size_t size;
        const VkDescriptorType type;
        const VkShaderStageFlags stage;

    public:
        ShaderDescriptor(uint32_t binding, size_t size, VkDescriptorType type, VkShaderStageFlags stage);

        [[nodiscard]] uint32_t getBinding() const;

        [[nodiscard]] VkDescriptorType getType() const;

        [[nodiscard]] VkShaderStageFlags getStage() const;

        [[nodiscard]] size_t getSize() const;
    };
}
