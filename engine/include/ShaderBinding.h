#ifndef VIXENENGINE_SHADERBINDING_H
#define VIXENENGINE_SHADERBINDING_H

#include <cstdio>
#include <vulkan/vulkan.h>

namespace Vixen {
    class ShaderBinding {
        const size_t size;
        const VkVertexInputBindingDescription binding;

    public:
        ShaderBinding(size_t size, const VkVertexInputBindingDescription &binding);

        [[nodiscard]] size_t getSize() const;

        [[nodiscard]] const VkVertexInputBindingDescription &getBinding() const;
    };
}

#endif