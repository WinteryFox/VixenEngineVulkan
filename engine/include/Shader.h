#ifndef VIXENENGINE_SHADER_H
#define VIXENENGINE_SHADER_H

#include <ranges>
#include "ShaderModule.h"

namespace Vixen {
    class Shader {
        const std::vector<const ShaderModule*> modules;

    public:
        explicit Shader(std::vector<const ShaderModule*> modules);

        [[nodiscard]] const std::vector<const ShaderModule*> &getModules() const;
        
        [[nodiscard]] std::vector<VkVertexInputBindingDescription> getAllBindings() const;

        [[nodiscard]] std::vector<VkVertexInputAttributeDescription> getAllAttributes() const;

        class Builder {
            std::vector<const ShaderModule*> modules{};

        public:
            Builder &addModule(const ShaderModule &module) {
                modules.push_back(&module);
                return *this;
            }

            [[nodiscard]] Shader build() const {
                return Shader(modules);
            }
        };
    };
}

#endif