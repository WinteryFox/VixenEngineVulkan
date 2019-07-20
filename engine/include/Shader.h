#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
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
         * The logical device this shader is made by
         */
        LogicalDevice *device;

        /**
         * Create a new Vulkan shader module
         *
         * @param[in] device The logical device to make the shader for
         * @param[in] filePath The path to the shader bytecode
         */
        Shader(LogicalDevice *device, const std::string &filePath);

        ~Shader();

    private:
        VkShaderModule createShader(LogicalDevice *logicalDevice, const std::string &filePath);
    };
}