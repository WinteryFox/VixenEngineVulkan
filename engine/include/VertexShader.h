#pragma once

#include "Shader.h"

namespace Vixen {
    struct VertexShader : Shader {
        struct ModelViewProjection {
            alignas(16) glm::mat4 model{};
            alignas(16) glm::mat4 view{};
            alignas(16) glm::mat4 projection{};
        } mvp;

        VertexShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath) : Shader(
                logicalDevice, filePath) {}

        VertexShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode) : Shader(
                logicalDevice, bytecode) {}
    };
}