#include "Shader.h"

#include <utility>

namespace Vixen {
    Shader::Shader(std::vector<std::shared_ptr<const ShaderModule>> modules, std::vector<ShaderBinding> bindings,
                   std::vector<VkVertexInputAttributeDescription> attributes) : modules(std::move(modules)),
                                                                                       bindings(std::move(bindings)),
                                                                                       attributes(std::move(attributes)) {}

    const std::vector<std::shared_ptr<const ShaderModule>> &Shader::getModules() const {
        return modules;
    }

    const std::vector<ShaderBinding> &Shader::getBindings() const {
        return bindings;
    }

    const std::vector<VkVertexInputAttributeDescription> &Shader::getAttributes() const {
        return attributes;
    }
}