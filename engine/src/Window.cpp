#include "Window.h"

namespace vixen {
    Window::Window(const std::string &name, bool fullscreen, int width, int height) {
        glfwInit();
        
        if (glfwVulkanSupported() != GLFW_TRUE)
            Logger().fatal("Vulkan is not supported. Did you install the SDK correctly?");
        
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        
        // Disable V-Sync
        glfwSwapInterval(0);
        
        // Create window
        window = glfwCreateWindow(width, height, name.c_str(), fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            Logger().fatal("Failed to create the window!");
        }

        // Centralize the window on the screen
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
        
        // Make the window visible
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