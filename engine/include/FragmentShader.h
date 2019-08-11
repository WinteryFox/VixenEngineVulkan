#pragma once

#include "Shader.h"

namespace Vixen {
    struct FragmentShader : Shader {
        FragmentShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath) : Shader(
                logicalDevice, filePath) {}

        FragmentShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode) : Shader(
                logicalDevice, bytecode) {}
    };
}