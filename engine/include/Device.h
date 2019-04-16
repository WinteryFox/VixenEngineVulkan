#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <memory>
#include <optional>
#include <vector>
#include <set>
#include "Instance.h"

namespace vixen {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        
        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    
    class Device {
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice logicalDevice;
        
        QueueFamilyIndices indices;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
    
        VkSwapchainKHR swapChain;
        SwapChainSupportDetails swapChainSupportDetails;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        VkFormat swapChainFormat;
        VkExtent2D swapChainExtent;
        
        std::string deviceName;
    
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
    
        const std::vector<const char *> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        
        void pickPhysicalDevice(VkSurfaceKHR surface, Instance *instance);
    
        bool isDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device);
    
        void createLogicalDevice();
    
        bool checkDeviceExtensionSupported(VkPhysicalDevice device);
        
        void findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device);
    
        void querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice device);
    
        void createSwapChain(VkSurfaceKHR surface, GLFWwindow *window);
    
        VkPresentModeKHR chooseSwapPresentMode(VkSurfaceKHR surface);
    
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    
        VkExtent2D chooseSwapExtent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR &capabilities);
    
        void createImageViews();
        
    public:
        explicit Device(GLFWwindow *window, VkSurfaceKHR surface, Instance *instance);
        
        ~Device();
    };
}