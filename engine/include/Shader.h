#pragma once

#include <vulkan/vulkan.h>
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
        VkShaderModule shader;

        /**
         * The logical device this shader is made by, required to be stored to destroy the shader
         */
        const std::shared_ptr<LogicalDevice> device;

        /**
         * Create a new Vulkan shader module
         *
         * @param[in] device The logical device to make the shader for
         * @param[in] filePath The path to the shader bytecode
         */
        Shader(const std::shared_ptr<LogicalDevice> &device, const std::string &filePath);

        ~Shader();

    private:
        VkShaderModule createShader(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::string &filePath);
    };
}