#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <memory>
#include "Logger.h"
#include "LogicalDevice.h"

namespace Vixen {
    class Shader {
    public:
        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        /**
         * The vertex shader module
         */
        VkShaderModule shader;

        /**
         * Create a new shader module
         *
         * @param[in] logicalDevice The device to make the shader for
         * @param[in] filePath The path to the file containing the shader bytecode
         */
        Shader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath);

        /**
         * Create a new shader module
         *
         * @param[in] logicalDevice The device to make the shader for
         * @param[in] bytecode The source bytecode of the shader
         */
        Shader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode);

        ~Shader();

    private:
        static std::vector<char> readBytecode(const std::string &filePath);
    };
}