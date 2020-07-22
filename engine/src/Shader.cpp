#include "Shader.h"

namespace Vixen {
    Shader::Shader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath)
            : logicalDevice(logicalDevice), shader(createShader(filePath)) {}

    Shader::Shader(const std::unique_ptr<LogicalDevice> &logicalDevice,
                   const std::vector<char> &bytecode)
            : logicalDevice(logicalDevice), shader(createShader(bytecode)) {}

    Shader::~Shader() {
        vkDestroyShaderModule(logicalDevice->device, shader, nullptr);
    }

    VkShaderModule Shader::createShader(const std::string &filePath) {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            error(IOException("Failed to open shader", filePath));
            return VK_NULL_HANDLE;
        }

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return createShader(buffer);
    }

    VkShaderModule Shader::createShader(const std::vector<char> &bytecode) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode.data());

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &shaderModule);
        if (result != VK_SUCCESS) {
            error(VulkanException("Failed to create shader module", result));
            return VK_NULL_HANDLE;
        }
        trace("Successfully created shader module");

        return shaderModule;
    }
}