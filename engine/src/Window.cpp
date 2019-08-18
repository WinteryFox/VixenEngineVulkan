#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"

namespace Vixen {
    Window::Window(const std::string &name, const std::string &icon, GLFWmonitor *monitor, int width, int height) {
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
        const auto primary = glfwGetPrimaryMonitor();
        const auto mode = glfwGetVideoMode(primary);
        int x, y;
        glfwGetMonitorPos(primary, &x, &y);
        glfwSetWindowPos(window, x + mode->width / 2 - width / 2, y + mode->height / 2 - height / 2);

        /// Load the window icon and set it
        setIcon(icon);

        /// Set mouse cursor disabled and enable raw input
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

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

    bool Window::setIcon(const std::string &path) {
        int width, height, channels;
        stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            glfwTerminate();
            error("Failed to load the window icon");
            return false;
        }

        GLFWimage image;
        image.pixels = pixels;
        image.width = width;
        image.height = height;
        glfwSetWindowIcon(window, 1, &image);

        stbi_image_free(pixels);
        return true;
    }
}