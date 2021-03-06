#include "Instance.h"

namespace Vixen {
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            void *pUserData
    ) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                trace("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                info("[VULKAN] " + std::string(pCallbackData->pMessage));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
#ifdef VIXEN_DEBUG
                fatal("[VULKAN] " + std::string(pCallbackData->pMessage));
#else
                warning("[VULKAN] " + std::string(pCallbackData->pMessage));
#endif
                break;
            default:
                break;
        }

        return VK_FALSE;
    }

    Instance::Instance(
            const Window &window,
            const std::string &appName,
            const glm::ivec3 appVersion,
            const std::vector<const char *> &requiredExtensions,
            const std::vector<const char *> &requiredLayers
    ) {
        queryExtensions(); /// Query and save the supported extensions
        queryLayers(); /// Query and save the supported layers
        checkExtensions(requiredExtensions);
        checkLayers(requiredLayers);
        createInstance(window, appName, appVersion, requiredExtensions, requiredLayers); /// Create the Vulkan instance
        registerLogger();
    }

    Instance::~Instance() {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    void Instance::createInstance(
            const Window &window,
            const std::string &appName,
            glm::ivec3 appVersion,
            const std::vector<const char *> &requiredExtensions,
            const std::vector<const char *> &requiredLayers
    ) {
        /// Fetch extensions for instance info
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> totalExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (const char *extension : requiredExtensions)
            totalExtensions.push_back(extension);

        /// The application info
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.x, appVersion.y, appVersion.z);
        appInfo.pEngineName = "Vixen Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(VIXEN_ENGINE_VERSION_MAJOR, VIXEN_ENGINE_VERSION_MINOR,
                                                VIXEN_ENGINE_VERSION_PATCH);
        appInfo.apiVersion = VIXEN_ENGINE_VULKAN_VERSION;

        /// The instance info
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = totalExtensions.size();
        createInfo.ppEnabledExtensionNames = totalExtensions.data();
        createInfo.enabledLayerCount = requiredLayers.size();
        createInfo.ppEnabledLayerNames = requiredLayers.data();

        enabledExtensions = totalExtensions;
        enabledLayers = requiredLayers;

        /// Attempt to create the instance, throws runtime error if no instance could be made
        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
            fatal("Failed to create Vulkan instance, extension not present!");
        else if (result == VK_ERROR_LAYER_NOT_PRESENT)
            fatal("Failed to create Vulkan instance, layer not present!");
        else if (result != VK_SUCCESS)
            fatal("Failed to create Vulkan instance, unknown error!");

        if (glfwCreateWindowSurface(instance, window.window, nullptr, &surface) != VK_SUCCESS)
            fatal("Failed to create window surface");
        trace("Successfully created the window surface");

        info("Vixen Engine version "
             + std::to_string(VK_VERSION_MAJOR(appInfo.engineVersion)) + "."
             + std::to_string(VK_VERSION_MINOR(appInfo.engineVersion)) + "."
             + std::to_string(VK_VERSION_PATCH(appInfo.engineVersion))
             + " running Vulkan version "
             + std::to_string(VK_VERSION_MAJOR(appInfo.apiVersion)) + "."
             + std::to_string(VK_VERSION_MINOR(appInfo.apiVersion)) + "."
             + std::to_string(VK_VERSION_PATCH(appInfo.apiVersion)));
        info("Serving application "
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
    }

    void Instance::queryExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        availableExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
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

            fatal(error);
        }
    }

    void Instance::checkLayers(const std::vector<const char *> &requiredLayers) {
        std::vector<const char *> result;

        std::vector<const char *> layerNames;
        for (const auto &layer : availableLayers)
            layerNames.push_back(layer.layerName);

        for (const auto &layer : requiredLayers)
            if (std::find(layerNames.begin(), layerNames.end(), layer) != layerNames.end())
                result.push_back(layer);

        std::string error = "Missing requested and required layers; ";
        if (!result.empty()) {
            for (const char *extension : result)
                error += std::string(extension) + " ";

            fatal(error);
        }
    }

    void Instance::registerLogger() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
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
            warning("Validation layers not present, Vulkan validation output is disabled!");
        }
    }
}