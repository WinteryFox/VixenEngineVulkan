#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <memory>
#include "Logger.h"
#include "LogicalDevice.h"

namespace Vixen {
    class ShaderModule {
        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        /**
         * The vertex shader module
         */
        VkShaderModule module;

        const VkShaderStageFlagBits stage;

        const std::string entryPoint;

        const std::vector<VkVertexInputBindingDescription> bindings;

        const std::vector<VkVertexInputAttributeDescription> attributes;

        static VkShaderModule
        createModule(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode);

        class ShaderBinding {
            std::type_info type;

        public:
            template<typename T>
            ShaderBinding() : type(typeid(T)) {}
        };

    public:
        /**
         * Create a new shader module
         *
         * @param[in] logicalDevice The device to make the shader for
         * @param[in] bytecode The source bytecode of the shader
         */
        ShaderModule(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode,
                     VkShaderStageFlagBits stage, std::string entryPoint,
                     std::vector<VkVertexInputBindingDescription> bindings,
                     std::vector<VkVertexInputAttributeDescription> attributes);

        ShaderModule(ShaderModule &&) = delete;

        ~ShaderModule();

        [[nodiscard]] const VkShaderModule &getModule() const;

        [[nodiscard]] const std::string &getEntryPoint() const;

        [[nodiscard]] const VkShaderStageFlagBits &getStage() const;

        [[nodiscard]] const std::vector<VkVertexInputBindingDescription> &getBindings() const;

        [[nodiscard]] const std::vector<ShaderBinding> &getAttributes() const;

        class Builder {
            const std::unique_ptr<LogicalDevice> &logicalDevice;

            std::vector<char> bytecode{};

            VkShaderStageFlagBits stage{};

            std::string entryPoint = "main";

            std::vector<VkVertexInputBindingDescription> bindings{};

            std::vector<VkVertexInputAttributeDescription> attributes{};

        public:
            explicit Builder(const std::unique_ptr<LogicalDevice> &logicalDevice) : logicalDevice(logicalDevice) {}

            Builder &setBytecode(std::vector<char> bytes) {
                bytecode = std::move(bytes);
                return *this;
            }

            Builder &setBytecode(const std::string &path) {
                std::ifstream file(path, std::ios::ate | std::ios::binary);
                if (!file.is_open())
                    error(IOException("Failed to open shader", path));

                size_t fileSize = (size_t) file.tellg();
                bytecode.resize(fileSize);
                file.seekg(0);
                file.read(bytecode.data(), fileSize);
                file.close();

                return *this;
            }

            Builder &setShaderStage(const VkShaderStageFlagBits &s) {
                stage = s;
                return *this;
            }

            Builder &setEntryPoint(const std::string &e) {
                entryPoint = e;
                return *this;
            }

            template<typename T>
            Builder &addBinding(uint32_t binding, VkVertexInputRate rate, uint32_t stride) {
                VkVertexInputBindingDescription input{};
                input.binding = binding;
                input.inputRate = rate;
                input.stride = stride;

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

            [[nodiscard]] ShaderModule build() const {
                if (bytecode.empty())
                    error("Bytecode must not be empty");
                if (stage == 0)
                    error("Shader stage flags must not be 0");

                return ShaderModule(logicalDevice, bytecode, stage, entryPoint, bindings, attributes);
            }
        };
    };
}