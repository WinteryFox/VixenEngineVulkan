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
    
    class Device {
    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice logicalDevice;
        
        QueueFamilyIndices indices;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        
        std::string deviceName;
    
        const std::vector<const char *> validationLayers = {
                "VK_LAYER_LUNARG_standard_validation"
        };
        
        void pickPhysicalDevice(VkSurfaceKHR surface, Instance *instance);
        bool isDeviceSuitable(VkSurfaceKHR surface, VkPhysicalDevice device);
        void createLogicalDevice();
    
        void findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice device);
        
    public:
        explicit Device(VkSurfaceKHR surface, Instance *instance);
        ~Device();
    };
}