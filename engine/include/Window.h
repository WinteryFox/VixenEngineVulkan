#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Logger.h"

namespace vixen {
    class Window {
    private:
        GLFWwindow *window;

    public:
        explicit Window(const std::string &name, GLFWmonitor *monitor = nullptr, int width = 1280, int height = 720);
        
        ~Window();
        
        bool shouldClose();
        
        void update();
        
        void swap();
    };
}