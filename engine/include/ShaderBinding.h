#ifndef VIXENENGINE_SHADERBINDING_H
#define VIXENENGINE_SHADERBINDING_H

#include <cstdio>
#include <vulkan/vulkan.h>

namespace Vixen {
    class ShaderBinding {
        const size_t size;
        const VkDescriptorType type;
        const VkShaderStageFlagBits stage;
        const VkVertexInputBindingDescription binding;

    public:
        ShaderBinding(size_t size, VkDescriptorType type, VkShaderStageFlagBits stage, const VkVertexInputBindingDescription &binding);

        [[nodiscard]] VkDescriptorType getType() const;

        [[nodiscard]] VkShaderStageFlagBits getStage() const;

        [[nodiscard]] size_t getSize() const;

        [[nodiscard]] const VkVertexInputBindingDescription &getInputBinding() const;
    };
}

#endif