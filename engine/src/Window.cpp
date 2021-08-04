#define STB_IMAGE_IMPLEMENTATION

#include "Window.h"

namespace Vixen {
    Window::Window(const std::string &name, const std::string &icon, GLFWmonitor *monitor, int width, int height) {
        glfwSetErrorCallback([](int code, const char* message) {
            Logger{"GLFW"}.error("{} ({})", message, code);
        });

        if (glfwInit() != GLFW_TRUE)
            logger.critical("Failed to initialize GLFW");

        if (glfwVulkanSupported() != GLFW_TRUE)
            logger.critical("Vulkan is not supported, updating your graphics drivers may fix this.");

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        /// Create window
        window = glfwCreateWindow(width, height, name.c_str(), monitor, nullptr);
        if (!window) {
            glfwTerminate();
            logger.critical("GLFW failed to create the window!");
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
        glfwSetWindowShouldClose(window, GLFW_TRUE); // TODO: Sus to destroy without waiting on actual closure
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(window) == GLFW_TRUE;
    }

    void Window::update() {
        glfwPollEvents();
    }

    void Window::setIcon(const std::string &path) const {
        int width, height, channels;
        stbi_uc *pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            glfwTerminate();
            throw std::runtime_error("Failed to load the window icon");
        }

        GLFWimage image;
        image.pixels = pixels;
        image.width = width;
        image.height = height;
        glfwSetWindowIcon(window, 1, &image);

        stbi_image_free(pixels);
    }
}