#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace vixen {
    class Instance {
    public:
        std::vector<VkExtensionProperties> extensions;

        Instance(const std::string& appName, glm::ivec3 appVersion);

    private:
        VkInstance instance;
    };
}