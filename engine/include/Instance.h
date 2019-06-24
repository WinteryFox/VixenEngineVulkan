#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>

namespace vixen {
    class Instance {
    public:
        VkInstance instance;
        std::vector<VkExtensionProperties> extensions;
        std::vector<VkLayerProperties> layers;

        Instance(
                const std::string &appName,
                glm::ivec3 appVersion,
                const std::vector<const char *> &requiredExtensions,
                const std::vector<const char *> &requiredLayers
        );

    private:
        void queryLayers();

        void queryExtensions();

        void createInstance(
                const std::string &appName,
                glm::ivec3 appVersion,
                const std::vector<const char *> &requiredExtensions,
                const std::vector<const char *> &requiredLayers
        );

        void checkExtensions(const std::vector<const char *> &requiredExtensions);

        void checkLayers(const std::vector<const char *> &requiredLayers);
    };
}