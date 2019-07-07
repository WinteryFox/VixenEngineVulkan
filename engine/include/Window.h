#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Logger.h"

namespace vixen {
    class Window {
    private:
        /**
         * The GLFW3 window instance
         */
        GLFWwindow *window;

    public:
        /**
         * Creates a new GLFW3 window
         *
         * @param[in] name The display name of the window
         * @param[in] monitor The monitor the window should be fullscreen on or nullptr if fullscreen should not be enabled
         * @param[in] width The width of the window
         * @param[in] height The height of the window
         */
        explicit Window(const std::string &name, GLFWmonitor *monitor = nullptr, int width = 1280, int height = 720);
        
        ~Window();
        bool shouldClose();

        /**
         * Update the GLFW3 window, this polls GLFW3 events
         */
        void update();

        /**
         * Swaps the GLFW3 window display buffers, draws the current frame to the window
         */
        void swap();
    };
}