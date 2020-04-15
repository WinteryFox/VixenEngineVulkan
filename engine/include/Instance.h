#pragma once

#include <vulkan/vulkan.h>

#define VIXEN_ENGINE_VERSION_MAJOR 0
#define VIXEN_ENGINE_VERSION_MINOR 0
#define VIXEN_ENGINE_VERSION_PATCH 1
#define VIXEN_ENGINE_VULKAN_VERSION VK_API_VERSION_1_2;

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "Logger.h"
#include "Window.h"

namespace Vixen {
    class Instance {
    public:
        /**
         * The Vulkan instance
         */
        VkInstance instance = VK_NULL_HANDLE;

        /**
         * Contains all available extensions on this system
         */
        std::vector<VkExtensionProperties> availableExtensions;

        /**
         * Contains all available layers on this system
         */
        std::vector<VkLayerProperties> availableLayers;

        /**
         * Contains all the currently enabled extensions by this instance
         */
        std::vector<const char *> enabledExtensions;

        /**
         * Contains all the currently enabled layers by this instance
         */
        std::vector<const char *> enabledLayers;

        /**
         * The Vulkan window surface
         */
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        /**
         * Creates a new Vulkan instance
         *
         * @param[in] appName The name of the application or game
         * @param[in] appVersion The version or revision of the application or game
         * @param[in] requiredExtensions A list of required Vulkan extensions
         * @param[in] requiredLayers A list of required Vulkan layers
         * @throws Throws upon failure to create a Vulkan instance, can be thrown because of missing layers, extensions or Vulkan not being installed on the system.
         */
        Instance(const Window &window, const std::string &appName, glm::ivec3 appVersion,
                 const std::vector<const char *> &requiredExtensions,
                 const std::vector<const char *> &requiredLayers);

        ~Instance();

    private:
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        /**
         * Query what Vulkan layers are available, this is stored in the availableLayers class property
         */
        void queryLayers();

        /**
         * Query what Vulkan extensions are available, this is stored in the availableExtensions class property
         */
        void queryExtensions();

        /**
         * Creates a new Vulkan instance
         *
         * @param[in] appName The name of the application or game
         * @param[in] appVersion The version or revision of the application or game
         * @param[in] requiredExtensions A list of required Vulkan extensions
         * @param[in] requiredLayers A list of required Vulkan layers
         * @throws Throws upon failure to create a Vulkan instance, can be thrown because of missing layers, extensions or Vulkan not being installed on the system.
         */
        void createInstance(const Window &window, const std::string &appName, glm::ivec3 appVersion,
                            const std::vector<const char *> &requiredExtensions,
                            const std::vector<const char *> &requiredLayers);

        /**
         * Checks whether all extensions in requiredExtensions are available
         *
         * @param[in] requiredExtensions The extensions requested to be enabled
         */
        void checkExtensions(const std::vector<const char *> &requiredExtensions);

        /**
         * Checks whether all layers in requiredLayers are available
         *
         * @param[in] requiredLayers The layers requested to be enabled
         */
        void checkLayers(const std::vector<const char *> &requiredLayers);

        /**
         * Registers the Vulkan debug validation layer logger
         */
        void registerLogger();
    };
}