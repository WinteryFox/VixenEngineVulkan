#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>

namespace vixen {
    class Window {
    private:
        GLFWwindow *window;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        
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
    
    public:
        Window(const std::string &name, bool fullscreen, int width = 1960, int height = 1080);
        
        ~Window();
        
        bool shouldClose();
        
        void update();
        
        void swap();
    };
}