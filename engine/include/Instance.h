#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
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
    
        bool checkValidationLayerSupport();
    
        std::vector<const char *> getRequiredExtensions();
    
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
    
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void *pUserData
        );
        void setupDebug();
        
    public:
        Instance();
        ~Instance();
    
        VkInstance instance = VK_NULL_HANDLE;
    };
}