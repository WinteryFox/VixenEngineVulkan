#include "Shader.h"

#include <utility>

namespace Vixen::Vk {
    Shader::Shader(std::vector<std::shared_ptr<const ShaderModule>> modules,
                   std::vector<VkVertexInputBindingDescription> bindings,
                   std::vector<VkVertexInputAttributeDescription> attributes,
                   std::vector<ShaderDescriptor> descriptors)
            : modules(std::move(modules)),
              bindings(std::move(bindings)),
              attributes(std::move(attributes)),
              descriptors(std::move(descriptors)) {}

    const std::vector<std::shared_ptr<const ShaderModule>> &Shader::getModules() const {
        return modules;
    }

    const std::vector<VkVertexInputBindingDescription> &Shader::getBindings() const {
        return bindings;
    }

    const std::vector<VkVertexInputAttributeDescription> &Shader::getAttributes() const {
        return attributes;
    }

    const std::vector<ShaderDescriptor> &Shader::getDescriptors() const {
        return descriptors;
    }
}