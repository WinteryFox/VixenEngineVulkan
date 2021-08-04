#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <utility>
#include <vector>
#include <memory>
#include "LogicalDevice.h"
#include "ShaderDescriptor.h"

namespace Vixen {
    class ShaderModule {
        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::shared_ptr<const LogicalDevice> logicalDevice;

        /**
         * The vertex shader module
         */
        VkShaderModule module{};

        const VkShaderStageFlagBits stage;

        const std::string entryPoint;

    public:
        /**
         * Create a new shader module
         *
         * @param[in] logicalDevice The device to make the shader for
         * @param[in] bytecode The source bytecode of the shader
         */
        ShaderModule(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode,
                     VkShaderStageFlagBits stage, std::string entryPoint);

        ShaderModule(ShaderModule &&) = delete;

        ~ShaderModule();

        [[nodiscard]] const VkShaderModule &getModule() const;

        [[nodiscard]] const std::string &getEntryPoint() const;

        [[nodiscard]] const VkShaderStageFlagBits &getStage() const;

        class Builder {
            const std::shared_ptr<LogicalDevice> logicalDevice;

            std::vector<char> bytecode{};

            VkShaderStageFlagBits stage{};

            std::string entryPoint = "main";

        public:
            explicit Builder(std::shared_ptr<LogicalDevice> logicalDevice) : logicalDevice(std::move(logicalDevice)) {}

            Builder &setBytecode(std::vector<char> bytes) {
                bytecode = std::move(bytes);
                return *this;
            }

            Builder &setBytecode(const std::string &path) {
                std::ifstream file(path, std::ios::ate | std::ios::binary);
                if (!file.is_open())
                    throw std::runtime_error("Failed to open shader");

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

            [[nodiscard]] std::shared_ptr<ShaderModule> build() const {
                if (bytecode.empty())
                    throw std::runtime_error("Bytecode must not be empty");
                if (stage == 0)
                    throw std::runtime_error("Shader stage flags must not be 0");

                return std::make_shared<ShaderModule>(logicalDevice, bytecode, stage, entryPoint);
            }
        };
    };
}