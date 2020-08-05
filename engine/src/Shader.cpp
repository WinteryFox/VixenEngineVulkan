#include "Shader.h"

namespace Vixen {
    Shader::Shader(std::vector<std::shared_ptr<const ShaderModule>> modules) : modules(std::move(modules)) {}

    const std::vector<std::shared_ptr<const ShaderModule>> &Shader::getModules() const {
        return modules;
    }

    std::vector<ShaderBinding> Shader::getAllBindings() const {
        std::vector<ShaderBinding> b{};

        for (const auto &module : modules)
            std::copy(module->getBindings().begin(), module->getBindings().end(), std::back_inserter(b));

        return b;
    }

    std::vector<VkVertexInputBindingDescription> Shader::getAllRawBindings() const {
        std::vector<VkVertexInputBindingDescription> b{};

        for (const auto &module : modules)
            for (const auto &binding : module->getBindings())
                b.push_back(binding.getBinding());

        return b;
    }

    std::vector<VkVertexInputAttributeDescription> Shader::getAllAttributes() const {
        std::vector<VkVertexInputAttributeDescription> a{};

        for (const auto &module : modules)
            std::copy(module->getAttributes().begin(), module->getAttributes().end(), std::back_inserter(a));

        return a;
    }
}