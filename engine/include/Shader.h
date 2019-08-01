#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <memory>
#include "Logger.h"
#include "LogicalDevice.h"

namespace Vixen {
    typedef enum ShaderType {
        SHADER_TYPE_VERTEX,
        SHADER_TYPE_FRAGMENT
    } ShaderType;

    class Shader {
    public:
        struct ModelViewProjection {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
        } mvp{};

        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        const ShaderType type;

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
        Shader(const std::unique_ptr<LogicalDevice> &logicalDevice, ShaderType type, const std::string &filePath);

        /**
         * Create a new shader module
         *
         * @param[in] logicalDevice The device to make the shader for
         * @param[in] bytecode The source bytecode of the shader
         */
        Shader(const std::unique_ptr<LogicalDevice> &logicalDevice, ShaderType type, const std::vector<char> &bytecode);

        ~Shader();

    private:
        VkShaderModule createShader(const std::string &filePath);

        VkShaderModule createShader(const std::vector<char> &bytecode);
    };
}