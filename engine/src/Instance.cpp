#include "Instance.h"

namespace vixen {
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
    ) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                Logger().trace("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                Logger().info("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Logger().warning("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Logger().error("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
        }

        return VK_FALSE;
    }

    Instance::Instance(
            const std::string &appName,
            const glm::ivec3 appVersion,
            const std::vector<const char *> &requiredExtensions,
            const std::vector<const char *> &requiredLayers
    ) {
        queryExtensions(); // Query and save the supported extensions
        queryLayers(); // Query and save the supported layers
        checkExtensions(requiredExtensions);
        createInstance(appName, appVersion, requiredExtensions, requiredLayers); // Create the Vulkan instance
        registerLogger();
    }

    Instance::~Instance() {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, nullptr);

        vkDestroyInstance(instance, nullptr);
    }

    void Instance::createInstance(
            const std::string &appName,
            glm::ivec3 appVersion,
            const std::vector<const char *> &requiredExtensions,
            const std::vector<const char *> &requiredLayers
    ) {
        // Fetch extensions for instance info
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> totalExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (const char *extension : requiredExtensions)
            totalExtensions.push_back(extension);

        // The application info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.x, appVersion.y, appVersion.z);
        appInfo.pEngineName = "Vixen Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        // The instance info
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = totalExtensions.size();
        createInfo.ppEnabledExtensionNames = totalExtensions.data();
        createInfo.enabledLayerCount = requiredLayers.size();
        createInfo.ppEnabledLayerNames = requiredLayers.data();

        enabledExtensions = totalExtensions;
        enabledLayers = requiredLayers;

        // Attempt to create the instance, throws runtime error if no instance could be made
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result == VK_ERROR_EXTENSION_NOT_PRESENT) {
            throw std::runtime_error("Failed to create Vulkan instance, extension not present!");
        } else if (result == VK_ERROR_LAYER_NOT_PRESENT) {
            throw std::runtime_error("Failed to create Vulkan instance, layer not present!");
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Vulkan instance, unknown error!");
        }

        Logger().info("Vixen Engine version "
                      + std::to_string(VK_VERSION_MAJOR(appInfo.engineVersion)) + "."
                      + std::to_string(VK_VERSION_MINOR(appInfo.engineVersion)) + "."
                      + std::to_string(VK_VERSION_PATCH(appInfo.engineVersion))
                      + " running Vulkan version "
                      + std::to_string(VK_VERSION_MAJOR(appInfo.apiVersion)) + "."
                      + std::to_string(VK_VERSION_MINOR(appInfo.apiVersion)) + "."
                      + std::to_string(VK_VERSION_PATCH(appInfo.apiVersion)));
        Logger().info("Serving application "
                      + std::string(appInfo.pApplicationName) + " "
                      + std::to_string(VK_VERSION_MAJOR(appVersion.x)) + "."
                      + std::to_string(VK_VERSION_MINOR(appVersion.y)) + "."
                      + std::to_string(VK_VERSION_PATCH(appVersion.z)));
    }

    void Instance::queryLayers() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        availableLayers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        std::string output(std::to_string(layerCount) + " layers available; ");
        for (auto const &layer : availableLayers)
            output += std::string(layer.layerName) + "(" +
                      std::to_string(VK_VERSION_MAJOR(layer.specVersion)) + "." +
                      std::to_string(VK_VERSION_MINOR(layer.specVersion)) + "." +
                      std::to_string(VK_VERSION_PATCH(layer.specVersion)) + ") ";
        Logger().trace(output);
    }

    void Instance::queryExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        availableExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        std::string output(std::to_string(extensionCount) + " extensions available; ");
        for (auto const &extension : availableExtensions)
            output += std::string(extension.extensionName) + "(" +
                      std::to_string(VK_VERSION_MAJOR(extension.specVersion)) + "." +
                      std::to_string(VK_VERSION_MINOR(extension.specVersion)) + "." +
                      std::to_string(VK_VERSION_PATCH(extension.specVersion)) + ") ";
        Logger().trace(output);
    }

    void Instance::checkExtensions(const std::vector<const char *> &requiredExtensions) {
        std::vector<const char *> result;

        std::vector<const char *> extensionNames;
        for (const auto &extension : availableExtensions)
            extensionNames.push_back(extension.extensionName);

        for (const auto &extension : requiredExtensions)
            if (std::find(extensionNames.begin(), extensionNames.end(), extension) !=
                extensionNames.end())
                result.push_back(extension);

        std::string error = "Missing requested and required extensions; ";
        if (!result.empty()) {
            for (const char *extension : result)
                error += std::string(extension) + " ";

            throw std::runtime_error(error);
        }
    }

    void Instance::checkLayers(const std::vector<const char *> &requiredLayers) {
        std::vector<const char *> result;

        std::vector<const char *> layerNames;
        for (const auto &layer : availableLayers)
            layerNames.push_back(layer.layerName);

        for (const auto &layer : requiredLayers)
            if (std::find(layerNames.begin(), layerNames.end(), layer) !=
                layerNames.end())
                result.push_back(layer);

        std::string error = "Missing requested and required layers; ";
        if (!result.empty()) {
            for (const char *extension : result)
                error += std::string(extension) + " ";

            throw std::runtime_error(error);
        }
    }

    void Instance::registerLogger() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        createInfo.pUserData = nullptr;

        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, &createInfo, nullptr, &debugMessenger);
        } else {
            std::cerr << "Validation layers not present, Vulkan debugging is OFF!" << std::endl;
        }
    }
}