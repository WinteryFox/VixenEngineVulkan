#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include "Instance.h"
#include "Device.h"

namespace vixen {
    class Window {
    private:
        GLFWwindow *window;
        VkSurfaceKHR surface{};
        
        Instance *instance;
        Device *device;
        
    public:
        Window(const std::string &name, bool fullscreen, int width = 1960, int height = 1080);
        
        ~Window();
        
        bool shouldClose();
        
        void update();
        
        void swap();
    };
}