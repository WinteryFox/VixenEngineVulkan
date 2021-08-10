#include "ShaderDescriptor.h"

namespace Vixen::Vk {
    ShaderDescriptor::ShaderDescriptor(uint32_t binding, size_t size, VkDescriptorType type, VkShaderStageFlags stage)
            : binding(binding), size(size), type(type), stage(stage) {}

    size_t ShaderDescriptor::getSize() const {
        return size;
    }

    VkDescriptorType ShaderDescriptor::getType() const {
        return type;
    }

    VkShaderStageFlags ShaderDescriptor::getStage() const {
        return stage;
    }

    uint32_t ShaderDescriptor::getBinding() const {
        return binding;
    }
}
