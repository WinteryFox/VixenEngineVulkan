#pragma once

#include <vulkan/vulkan.h>
//#include <glslang/Public/ShaderLang.h>
//#include <SPIRV/GlslangToSpv.h>
//#include <StandAlone/DirStackFileIncluder.h>
#include <fstream>
#include <vector>
#include <memory>
#include "Logger.h"
#include "LogicalDevice.h"

namespace vixen {
    class Shader {
    public:
        /**
         * The vertex shader module
         */
        VkShaderModule shader = VK_NULL_HANDLE;

        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::unique_ptr<LogicalDevice> &device;

        /**
         * Create a new Vulkan shader module
         *
         * @param[in] device The logical device to make the shader for
         * @param[in] filePath The path to the shader bytecode
         */
        Shader(const std::unique_ptr<LogicalDevice> &device, const std::string &filePath);

        Shader(const std::unique_ptr<LogicalDevice> &device, const std::vector<char> &bytecode);

        ~Shader();

    private:
        static VkShaderModule
        createShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath);

        static VkShaderModule
        createShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode);
    };

    //std::vector<char> compileShader(const std::string &filePath, EShLanguage kind);
}