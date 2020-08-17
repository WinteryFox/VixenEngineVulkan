#include "ShaderBinding.h"

namespace Vixen {
    ShaderBinding::ShaderBinding(size_t size, VkDescriptorType type, VkShaderStageFlagBits stage,
                                 const VkVertexInputBindingDescription &binding)
            : size(size), type(type), stage(stage), binding(binding) {}

    size_t ShaderBinding::getSize() const {
        return size;
    }

    const VkVertexInputBindingDescription &ShaderBinding::getInputBinding() const {
        return binding;
    }

    VkDescriptorType ShaderBinding::getType() const {
        return type;
    }

    VkShaderStageFlagBits ShaderBinding::getStage() const {
        return stage;
    }

    const VkVertexInputBindingDescription &ShaderBinding::getBinding() const {
        return binding;
    }
}
