#ifndef VIXENENGINE_SHADER_H
#define VIXENENGINE_SHADER_H

#include <range/v3/view.hpp>
#include "Util.h"
#include "ShaderModule.h"

namespace Vixen {
    class Shader {
        const std::vector<std::shared_ptr<const ShaderModule>> modules;

    public:
        explicit Shader(std::vector<std::shared_ptr<const ShaderModule>> modules);

        [[nodiscard]] const std::vector<std::shared_ptr<const ShaderModule>> &getModules() const;

        [[nodiscard]] std::vector<ShaderBinding> getAllBindings() const;

        [[nodiscard]] std::vector<VkVertexInputBindingDescription> getAllRawBindings() const;

        [[nodiscard]] std::vector<VkVertexInputAttributeDescription> getAllAttributes() const;

        class Builder {
            std::vector<std::shared_ptr<const ShaderModule>> modules{};

        public:
            Builder &addModule(const std::shared_ptr<const ShaderModule> &module) {
                modules.push_back(module);
                return *this;
            }

            [[nodiscard]] Shader build() const {
                return Shader(modules);
            }
        };
    };
}

#endif