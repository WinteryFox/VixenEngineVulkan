#include "Input.h"

namespace Vixen {
    Input::Input(const std::shared_ptr<Window> &window) : window(window) {
        glfwGetCursorPos(window->window, &lastX, &lastY);
    }

    void Input::update(Camera &camera, const double deltaTime) {
        if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window->window, GLFW_TRUE); /// Just a shortcut to exit the app

        /// Mouse input
        double currentX, currentY;
        double offsetX, offsetY;
        glfwGetCursorPos(window->window, &currentX, &currentY);
        offsetX = currentX - lastX;
        offsetY = currentY - lastY;
        lastX = currentX;
        lastY = currentY;

        camera.horizontal -= 0.01f * offsetX;
        camera.vertical = glm::clamp(camera.vertical - 0.01f * offsetY, -1.5, 1.5);

        camera.rotation = {
                cos(camera.vertical) * sin(camera.horizontal),
                sin(camera.vertical),
                cos(camera.vertical) * cos(camera.horizontal)
        };
        camera.right = {
                sin(camera.horizontal - std::numbers::pi / 2.0f),
                0,
                cos(camera.horizontal - std::numbers::pi / 2.0f)
        };

        /// Keyboard input
        glm::vec3 advance = {0.0f, 0.0f, 0.0f};
        if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
            advance += camera.rotation * speed;
        if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
            advance -= camera.rotation * speed;
        if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
            advance -= camera.right * speed;
        if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
            advance += camera.right * speed;
        if (glfwGetKey(window->window, GLFW_KEY_SPACE) == GLFW_PRESS)
            advance.y += speed;
        if (glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            advance.y -= speed;

        advance.x *= static_cast<float>(deltaTime);
        advance.y *= static_cast<float>(deltaTime);
        advance.z *= static_cast<float>(deltaTime);

        camera.position += advance;
    }
}