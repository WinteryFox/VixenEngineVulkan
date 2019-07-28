#include "Shader.h"

namespace vixen {
    Shader::Shader(const std::unique_ptr<LogicalDevice> &device, const std::string &filePath) : device(device) {
        shader = createShader(device, filePath);
    }

    Shader::Shader(const std::unique_ptr<LogicalDevice> &device, const std::vector<char> &bytecode) : device(device) {
        shader = createShader(device, bytecode);
    }

    Shader::~Shader() {
        vkDestroyShaderModule(device->device, shader, nullptr);
    }

    VkShaderModule
    Shader::createShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &filePath) {
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

        return createShader(logicalDevice, buffer);
    }

    VkShaderModule
    Shader::createShader(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<char> &bytecode) {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = bytecode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(bytecode.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(logicalDevice->device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            error("Failed to create shader module");
            return VK_NULL_HANDLE;
        }

        trace("Successfully created shader module");

        return shaderModule;
    }

    /*std::vector<char> compileShader(const std::string &filePath, EShLanguage kind) {
        std::FILE *file = std::fopen(filePath.c_str(), "rb");
        if (!file) {
            error("Failed to open shader " + filePath);
            return {};
        }

        std::string buffer;
        std::fseek(file, 0, SEEK_END);
        buffer.resize(std::ftell(file));
        std::rewind(file);
        std::fread(buffer.data(), 1, buffer.size(), file);
        std::fclose(file);

        const char* source = buffer.c_str();
        glslang::InitializeProcess();

        const TBuiltInResource DefaultTBuiltInResource = {};

        glslang::TShader shader(kind);
        shader.setStrings(&source, 1);

        int version = 110;
        glslang::EshTargetClientVersion clientVersion = glslang::EShTargetVulkan_1_1;
        glslang::EShTargetLanguageVersion targetVersion = glslang::EShTargetSpv_1_4;

        shader.setEnvInput(glslang::EShSourceGlsl, kind, glslang::EShClientVulkan, version);
        shader.setEnvClient(glslang::EShClientVulkan, clientVersion);
        shader.setEnvTarget(glslang::EShTargetSpv, targetVersion);

        TBuiltInResource resources = DefaultTBuiltInResource;
        auto messages = (EShMessages) (EShMsgSpvRules | EShMsgVulkanRules);

        DirStackFileIncluder includer;
        includer.pushExternalLocalDirectory(filePath.substr(0, filePath.find_last_of('/')));

        std::string preprocessed;
        if (!shader.preprocess(&resources, version, ENoProfile, false, false, messages, &preprocessed, includer)) {
            error("Failed to preprocess GLSL;\n" + std::string(shader.getInfoLog()));
            return {};
        }

        const char* preprocessedPtr = preprocessed.c_str();
        shader.setStrings(&preprocessedPtr, 1);

        if (!shader.parse(&resources, version, false, messages)) {
            error("Failed to parse shader;\n" + std::string(shader.getInfoLog()));
            return {};
        }

        glslang::TProgram program;
        program.addShader(&shader);

        if (!program.link(messages)) {
            error("Failed to link shader;\n" + std::string(shader.getInfoLog()));
            return {};
        }

        std::vector<unsigned int> bytecode;
        glslang::GlslangToSpv(*program.getIntermediate(kind), bytecode, nullptr, nullptr);

        glslang::FinalizeProcess();

        trace("Compiled shader " + filePath);

        return {bytecode.cbegin(), bytecode.cend()};
        return {};
    }*/
}