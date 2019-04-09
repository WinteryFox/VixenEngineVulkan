#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

namespace vixen {
    class Instance {
    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        
        std::string deviceName;
        
        bool debug = true;
    
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
    
        void createInstance();
    
        bool checkValidationLayerSupport();
    
        std::vector<const char *> getRequiredExtensions();
    
        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void *pUserData
        );
        void setupDebug();
        void pickPhysicalDevice();
        bool isDeviceSuitable(VkPhysicalDevice device);
        
    public:
        Instance();
        ~Instance();
    };
}