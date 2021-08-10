#pragma once

#include "ShaderModule.h"
#include "ShaderDescriptor.h"

namespace Vixen {
    class Shader {
        const std::vector<std::shared_ptr<const ShaderModule>> modules;
        const std::vector<VkVertexInputBindingDescription> bindings;
        const std::vector<VkVertexInputAttributeDescription> attributes;
        const std::vector<ShaderDescriptor> descriptors;

    public:
        explicit Shader(std::vector<std::shared_ptr<const ShaderModule>> modules,
                        std::vector<VkVertexInputBindingDescription> bindings,
                        std::vector<VkVertexInputAttributeDescription> attributes,
                        std::vector<ShaderDescriptor> descriptors);

        [[nodiscard]] const std::vector<std::shared_ptr<const ShaderModule>> &getModules() const;

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription> &getBindings() const;

        [[nodiscard]] const std::vector<VkVertexInputAttributeDescription> &getAttributes() const;

        [[nodiscard]] const std::vector<ShaderDescriptor> &getDescriptors() const;

        class Builder {
            std::vector<std::shared_ptr<const ShaderModule>> modules{};
            std::vector<VkVertexInputBindingDescription> bindings{};
            std::vector<VkVertexInputAttributeDescription> attributes{};
            std::vector<ShaderDescriptor> descriptors;

        public:
            Builder &addModule(const std::shared_ptr<const ShaderModule> &module) {
                modules.push_back(module);
                return *this;
            }

            Builder &addBinding(uint32_t binding, VkVertexInputRate rate, uint32_t stride) {
                VkVertexInputBindingDescription input{};
                input.binding = binding;
                input.stride = stride;
                input.inputRate = rate;

                bindings.push_back(input);
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

            Builder &addDescriptor(uint32_t binding, size_t size, VkDescriptorType type, VkShaderStageFlags flags) {
                descriptors.emplace_back(binding, size, type, flags);
                return *this;
            }

            [[nodiscard]] std::shared_ptr<Shader> build() const {
                return std::make_shared<Shader>(modules, bindings, attributes, descriptors);
            }
        };
    };
}
