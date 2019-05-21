#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>
#include <optional>

namespace vixen {
    class Instance {
    private:
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        
        void createInstance();
        
        std::vector<const char *> getRequiredExtensions();

#ifdef VIXEN_DEBUG
        bool checkValidationLayerSupport();
        
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
        
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData
        );
        
        void setupDebug();
#endif
    
    public:
        Instance();
        
        ~Instance();
        
        VkInstance instance = VK_NULL_HANDLE;
    };
}