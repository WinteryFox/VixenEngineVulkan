#include "Shader.h"

namespace Vixen {
    Shader::Shader(std::vector<const ShaderModule*> modules) : modules(std::move(modules)) {}

    const std::vector<const ShaderModule *> &Shader::getModules() const {
        return modules;
    }

    std::vector<VkVertexInputBindingDescription> Shader::getAllBindings() const {
        std::vector<VkVertexInputBindingDescription> b{};

        for (const auto &module : modules) {
            b.insert(std::end(b), std::begin(module->getBindings()), std::end(module->getBindings()));
        }

        return b;
    }

    std::vector<VkVertexInputAttributeDescription> Shader::getAllAttributes() const {
        /*std::vector<VkVertexInputAttributeDescription> a{};

        for (const auto &module : modules) {
            a.insert(std::end(a), std::begin(module->getAttributes()), std::end(module->getAttributes()));
        }

        return a;*/
        return modules | std::ranges::views::transform([](const ShaderModule *module) { return module->getAttributes(); });
    }
}