#include "ShaderModule.h"

namespace Vixen {
    ShaderModule::ShaderModule(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode,
                               VkShaderStageFlagBits stage, std::string entryPoint)
            : logicalDevice(logicalDevice), module(createModule(logicalDevice, bytecode)), stage(stage),
              entryPoint(std::move(entryPoint)) {}

    ShaderModule::~ShaderModule() {
        vkDestroyShaderModule(logicalDevice->device, module, nullptr);
    }

    VkShaderModule
    ShaderModule::createModule(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode.data());

        VkShaderModule module;
        VK_CHECK_RESULT(vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &module))
        trace("Successfully created shader module");

        return module;
    }

    const VkShaderModule &ShaderModule::getModule() const {
        return module;
    }

    const std::string &ShaderModule::getEntryPoint() const {
        return entryPoint;
    }

    const VkShaderStageFlagBits &ShaderModule::getStage() const {
        return stage;
    }
}