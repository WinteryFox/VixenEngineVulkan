#ifndef VIXENENGINE_SHADER_H
#define VIXENENGINE_SHADER_H

#include <range/v3/view.hpp>
#include "Util.h"
#include "ShaderModule.h"

namespace Vixen {
    class Shader {
        const std::vector<std::shared_ptr<const ShaderModule>> modules;
        const std::vector<ShaderBinding> bindings;
        const std::vector<VkVertexInputAttributeDescription> attributes;

    public:
        explicit Shader(std::vector<std::shared_ptr<const ShaderModule>> modules,
                        std::vector<ShaderBinding> bindings,
                        std::vector<VkVertexInputAttributeDescription> attributes);

        [[nodiscard]] const std::vector<std::shared_ptr<const ShaderModule>> &getModules() const;

        [[nodiscard]] const std::vector<ShaderBinding> &getBindings() const;

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> &getAttributes() const;

        class Builder {
            std::vector<std::shared_ptr<const ShaderModule>> modules{};
            std::vector<ShaderBinding> bindings{};
            std::vector<VkVertexInputAttributeDescription> attributes{};

        public:
            Builder &addModule(const std::shared_ptr<const ShaderModule> &module) {
                modules.push_back(module);
                return *this;
            }

            template<typename T>
            Builder &
            addBinding(VkDescriptorType type, VkShaderStageFlagBits stage, uint32_t binding, VkVertexInputRate rate,
                       uint32_t stride) {
                VkVertexInputBindingDescription input{};
                input.binding = binding;
                input.inputRate = rate;
                input.stride = stride;

                bindings.emplace_back(sizeof(T), type, stage, input);

                return *this;
            }

            Builder &addAttribute(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) {
                VkVertexInputAttributeDescription input{};
                input.binding = binding;
                input.location = location;
                input.format = format;
                input.offset = offset;

                attributes.push_back(input);

                return *this;
            }

            [[nodiscard]] Shader *build() const {
                return new Shader(modules, bindings, attributes);
            }
        };
    };
}

#endif