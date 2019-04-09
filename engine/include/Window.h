#pragma once

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "Instance.h"

namespace vixen {
    class Window {
    private:
        GLFWwindow *window;
        Instance *instance;
        
        bool debug = true;
    
    public:
        Window(const std::string &name, bool fullscreen, int width = 1960, int height = 1080);
        
        ~Window();
        
        bool shouldClose();
        
        void update();
        
        void swap();
    };
}