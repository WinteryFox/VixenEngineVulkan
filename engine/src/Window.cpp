#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"

namespace vixen {
    Window::Window(const std::string &name, GLFWmonitor *monitor, int width, int height) {
        glfwInit();

        if (glfwVulkanSupported() != GLFW_TRUE)
            fatal("Vulkan is not supported, updating your graphics drivers may fix this.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        /// Disable V-Sync
        glfwSwapInterval(0);

        /// Create window
        window = glfwCreateWindow(width, height, name.c_str(), monitor, nullptr);
        if (!window) {
            glfwTerminate();
            fatal("GLFW failed to create the window!");
        }

        /// Centralize the window on the screen
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);

        /// Load the window icon and set it
        int iconWidth, iconHeight, channels;
        stbi_uc *pixels = stbi_load("../../icon.png", &iconWidth, &iconHeight, &channels, 4);
        if (!pixels) {
            glfwTerminate();
            fatal("Failed to load the window icon");
        }

        GLFWimage icon;
        icon.pixels = pixels;
        icon.width = iconWidth;
        icon.height = iconHeight;
        glfwSetWindowIcon(window, 1, &icon);

        stbi_image_free(pixels);

        /// Make the window visible
        glfwShowWindow(window);
    }

    Window::~Window() {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::shouldClose() {
        return glfwWindowShouldClose(window) == GLFW_TRUE;
    }

    void Window::update() {
        glfwPollEvents();
    }

    void Window::swap() {
        glfwSwapBuffers(window);
    }
}