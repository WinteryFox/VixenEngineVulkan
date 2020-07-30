#include "Shader.h"

namespace Vixen {
    Shader::Shader(std::vector<std::shared_ptr<const ShaderModule>> modules) : modules(std::move(modules)) {}

    const std::vector<std::shared_ptr<const ShaderModule>> &Shader::getModules() const {
        return modules;
    }

    std::vector<ShaderBinding> Shader::getAllBindings() const {
        return modules |
               ranges::views::transform(
                       [](const std::shared_ptr<const ShaderModule> &module) { return module->getBindings(); }) |
               ranges::views::cache1 |
               ranges::views::join |
               ranges::to_vector;
    }

    std::vector<VkVertexInputAttributeDescription> Shader::getAllAttributes() const {
        return modules |
               ranges::views::transform(
                       [](const std::shared_ptr<const ShaderModule> &module) { return module->getAttributes(); }) |
               ranges::views::cache1 |
               ranges::views::join |
               ranges::to_vector;
    }
}