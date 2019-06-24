#include "Instance.h"

namespace vixen {
    Instance::Instance(
            const std::string &appName,
            glm::ivec3 appVersion,
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

        std::cout << "Vixen Engine version "
                  << VK_VERSION_MAJOR(appInfo.engineVersion) << "."
                  << VK_VERSION_MINOR(appInfo.engineVersion) << "."
                  << VK_VERSION_PATCH(appInfo.engineVersion)
                  << " running Vulkan version "
                  << VK_VERSION_MAJOR(appInfo.apiVersion) << "."
                  << VK_VERSION_MINOR(appInfo.apiVersion) << "."
                  << VK_VERSION_PATCH(appInfo.apiVersion) << std::endl;
        std::cout << "Serving application "
                  << appInfo.pApplicationName << " "
                  << VK_VERSION_MAJOR(appVersion.x) << "."
                  << VK_VERSION_MINOR(appVersion.y) << "."
                  << VK_VERSION_PATCH(appVersion.z) << std::endl;
    }

    void Instance::queryLayers() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        availableLayers.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        std::cout << layerCount << " layers available; ";
        for (auto const &layer : availableLayers)
            std::cout << layer.layerName << "(" <<
                      VK_VERSION_MAJOR(layer.specVersion) << "." <<
                      VK_VERSION_MINOR(layer.specVersion) << "." <<
                      VK_VERSION_PATCH(layer.specVersion) << ") ";
        std::cout << std::endl;
    }

    void Instance::queryExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        availableExtensions.resize(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
        std::cout << extensionCount << " extensions available; ";
        for (auto const &ext : availableExtensions)
            std::cout << ext.extensionName << "(" <<
                      VK_VERSION_MAJOR(ext.specVersion) << "." <<
                      VK_VERSION_MINOR(ext.specVersion) << "." <<
                      VK_VERSION_PATCH(ext.specVersion) << ") ";
        std::cout << std::endl;
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


    }
}