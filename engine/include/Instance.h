#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "Logger.h"

namespace vixen {
    class Instance {
    public:
        VkInstance instance;
        std::vector<VkExtensionProperties> availableExtensions;
        std::vector<VkLayerProperties> availableLayers;
        std::vector<const char *> enabledExtensions;
        std::vector<const char *> enabledLayers;

        Instance(const std::string &appName, glm::ivec3 appVersion, const std::vector<const char *> &requiredExtensions,
                 const std::vector<const char *> &requiredLayers);

        ~Instance();

    private:
        VkDebugUtilsMessengerEXT debugMessenger;

        void queryLayers();

        void queryExtensions();

        void createInstance(const std::string &appName, glm::ivec3 appVersion,
                            const std::vector<const char *> &requiredExtensions,
                            const std::vector<const char *> &requiredLayers);

        void checkExtensions(const std::vector<const char *> &requiredExtensions);

        void checkLayers(const std::vector<const char *> &requiredLayers);

        void registerLogger();
    };
}