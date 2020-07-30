#include "ShaderBinding.h"

namespace Vixen {
    ShaderBinding::ShaderBinding(const size_t size, const VkVertexInputBindingDescription &binding) : size(size),
                                                                                                      binding(binding) {}

    size_t ShaderBinding::getSize() const {
        return size;
    }

    const VkVertexInputBindingDescription &ShaderBinding::getBinding() const {
        return binding;
    }
}
