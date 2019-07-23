#include "Shader.h"

namespace vixen {
    Shader::Shader(const std::shared_ptr<LogicalDevice> &device, const std::string &filePath) : device(device) {
        shader = createShader(device, filePath);
    }

    Shader::~Shader() {
        vkDestroyShaderModule(device->device, shader, nullptr);
    }

    VkShaderModule
    Shader::createShader(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::string &filePath) {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            error("Failed to open shader " + filePath);
            return VK_NULL_HANDLE;
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = buffer.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(buffer.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            error("Failed to create shader module " + filePath);
            return VK_NULL_HANDLE;
        }

        trace("Successfully created shader module " + filePath);

        return shaderModule;
    }
}