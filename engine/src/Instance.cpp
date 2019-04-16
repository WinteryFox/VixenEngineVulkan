#include <Instance.h>

#ifdef VIXEN_DEBUG
VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                      const VkAllocationCallbacks *pAllocator,
                                      VkDebugUtilsMessengerEXT *pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
#endif

namespace vixen {
    Instance::Instance() {
        createInstance();
#ifdef VIXEN_DEBUG
        setupDebug();
#endif
    }
    
    Instance::~Instance() {
#ifdef VIXEN_DEBUG
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);
    }
    
    /**
     * Create the Vulkan instance
     */
    void Instance::createInstance() {
#ifdef VIXEN_DEBUG
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif
        
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef VIXEN_DEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
#else
        createInfo.enabledLayerCount = 0;
#endif
        
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

#ifdef VIXEN_DEBUG
    /**
     * Check if the requested validation layers are supported
     */
    bool Instance::checkValidationLayerSupport() {
        uint32_t layerCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &layerCount, nullptr);
        
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        
        for (const char *layerName : validationLayers) {
            bool layerFound = false;
            
            for (const auto &layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }
            
            if (!layerFound)
                return false;
        }
        
        return true;
    }
#endif
    
    std::vector<const char *> Instance::getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        
        std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef VIXEN_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
        
        return extensions;
    }

#ifdef VIXEN_DEBUG
    VkBool32 Instance::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
        VkDebugUtilsMessageSeverityFlagBitsEXT severity;

#if VIXEN_DEBUG_LEVEL == 0
        severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
#elif VIXEN_DEBUG_LEVEL == 1
        severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
#elif VIXEN_DEBUG_LEVEL == 2
        severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
#elif VIXEN_DEBUG_LEVEL == 3
       severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
#endif
        if (messageSeverity >= severity) {
            switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    std::cout << "Source: VERBOSE";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    std::cout << "Source: INFO";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    std::cout << "Source: WARNING";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    std::cout << "Source: ERROR";
                    break;
                default:
                    break;
            }
            std::cout << std::endl;
            
            switch (messageType) {
                case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
                    std::cout << "Type: GENERAL";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
                    std::cout << "Type: VALIDATION";
                    break;
                case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
                    std::cout << "Type: PERFORMANCE";
                    break;
                default:
                    break;
            }
            std::cout << std::endl;
            
            std::cout << pCallbackData->pMessage << std::endl;
            std::cout << "---------------------" << std::endl;
        }
        
        return VK_FALSE;
    }
#endif

#ifdef VIXEN_DEBUG
    void Instance::setupDebug() {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
        
        if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("Failed to set up debug messenger!");
        }
    }
#endif
}