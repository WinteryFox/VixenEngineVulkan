#pragma once

#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "Logger.h"
#include "src/exceptions/IOException.h"

namespace Vixen {
    class Window {
    public:
        /**
         * The GLFW3 window instance
         */
        GLFWwindow *window;

        /**
         * Creates a new GLFW3 window
         *
         * @param[in] name The display name of the window
         * @param[in] icon The icon to set for the window
         * @param[in] monitor The monitor the window should be fullscreen on or nullptr if fullscreen should not be enabled
         * @param[in] width The width of the window
         * @param[in] height The height of the window
         */
        explicit Window(const std::string &name, const std::string &icon,
                        GLFWmonitor *monitor = nullptr, int width = 1280, int height = 720);

        ~Window();

        [[nodiscard]] bool shouldClose() const;

        /**
         * Update the GLFW3 window, this polls GLFW3 events
         */
        static void update();

        void setIcon(const std::string &path) const;
    };
}