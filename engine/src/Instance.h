#pragma once

#include <vulkan/vulkan.h>

inline constexpr int VIXEN_ENGINE_VERSION_MAJOR = 0;
inline constexpr int VIXEN_ENGINE_VERSION_MINOR = 0;
inline constexpr int VIXEN_ENGINE_VERSION_PATCH = 1;
inline constexpr int VIXEN_ENGINE_VULKAN_VERSION = VK_API_VERSION_1_2;

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <algorithm>
#include "Window.h"
#include "Vulkan.h"

namespace Vixen {
    class Instance {
        Logger logger{"Instance"};

#ifdef VIXEN_DEBUG
        VkDebugUtilsMessengerEXT debugMessenger{};
#endif

        const std::shared_ptr<Window> window;

        VkInstance instance{};

        VkSurfaceKHR surface{};

    public:
        Instance(const std::shared_ptr<Window> &window, const std::string &appName, glm::ivec3 appVersion);

        Instance(const Instance &other) = delete;

        ~Instance();

        [[nodiscard]] VkInstance getInstance() const;

        [[nodiscard]] VkSurfaceKHR getSurface() const;
    };
}