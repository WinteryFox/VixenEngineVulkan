#include "Instance.h"

namespace Vixen {
#ifdef VIXEN_DEBUG

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
            [[maybe_unused]] void *pUserData
    ) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                Logger("Vulkan").trace("{}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                Logger{"Vulkan"}.info("{}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                Logger{"Vulkan"}.warning("{}", pCallbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                Logger{"Vulkan"}.error("{}", pCallbackData->pMessage);
                break;
            default:
                break;
        }

        return VK_FALSE;
    }

#endif

    Instance::Instance(const std::shared_ptr<Window> &window, const std::string &appName, const glm::ivec3 appVersion)
            : window(window) {
        uint32_t glfwExtensionCount;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#ifdef VIXEN_DEBUG
        extensions.push_back("VK_EXT_debug_report");
        extensions.push_back("VK_EXT_debug_utils");
#endif

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appVersion.x, appVersion.y, appVersion.z);
        appInfo.pEngineName = "Vixen Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, VIXEN_ENGINE_VERSION_MAJOR, VIXEN_ENGINE_VERSION_MINOR,
                                                    VIXEN_ENGINE_VERSION_PATCH);
        appInfo.apiVersion = VIXEN_ENGINE_VULKAN_VERSION;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();
#ifndef VIXEN_DEBUG
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
#else
        std::vector<const char *> l{"VK_LAYER_KHRONOS_validation"};

        createInfo.enabledLayerCount = l.size();
        createInfo.ppEnabledLayerNames = l.data();

        VkDebugUtilsMessengerCreateInfoEXT debugInfo = {};
        debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugInfo.pfnUserCallback = debugCallback;
        debugInfo.pUserData = nullptr;

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugInfo;
#endif

        VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance))
        logger.info("Vixen Engine v{}.{}.{} running Vulkan v{}.{}.{}",
                    VK_VERSION_MAJOR(appInfo.engineVersion), VK_VERSION_MINOR(appInfo.engineVersion),
                    VK_VERSION_PATCH(appInfo.engineVersion), VK_VERSION_MAJOR(appInfo.apiVersion),
                    VK_VERSION_MINOR(appInfo.apiVersion), VK_VERSION_PATCH(appInfo.apiVersion));
        logger.info("Serving application \"{}\" v{}.{}.{}", appInfo.pApplicationName, appVersion.x, appVersion.y,
                    appVersion.z);

#ifdef VIXEN_DEBUG
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                               "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, &debugInfo, nullptr, &debugMessenger);
            logger.debug("Vulkan validation output has been enabled");
        } else {
            logger.warning("Debug mode is enabled but the Vulkan validation layer is not present, install it?");
        }
#endif

        VK_CHECK_RESULT(glfwCreateWindowSurface(instance, window->window, nullptr, &surface))
    }

    Instance::~Instance() {
#ifdef VIXEN_DEBUG
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, nullptr);
#endif

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);
    }

    VkInstance Instance::getInstance() const {
        return instance;
    }

    VkSurfaceKHR Instance::getSurface() const {
        return surface;
    }
}