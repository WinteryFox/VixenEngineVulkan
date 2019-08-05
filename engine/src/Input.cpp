#include "Input.h"

namespace Vixen {
    Input::Input(const std::unique_ptr<Window> &window) : window(window) {
        glfwGetCursorPos(window->window, &lastX, &lastY);
    }

    void Input::update(const std::unique_ptr<Camera> &camera) {
        /// Break out of focus by pressing escape and back into focus if it is hit again
        if (glfwGetKey(window->window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (glfwGetInputMode(window->window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(window->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        /// Mouse input
        double currentX, currentY;
        double offsetX, offsetY;
        glfwGetCursorPos(window->window, &currentX, &currentY);
        offsetX = currentX - lastX;
        offsetY = currentY - lastY;
        lastX = currentX;
        lastY = currentY;

        horizontal -= 0.01 * offsetX;
        vertical -= 0.01 * offsetY;

        camera->rotation = {cos(vertical) * sin(horizontal), sin(vertical), cos(vertical) * cos(horizontal)};
        camera->right = {sin(horizontal - 3.14f / 2.0f), 0, cos(horizontal - 3.14f / 2.0f)};

        /// Keyboard input
        glm::vec3 advance = {0.0f, 0.0f, 0.0f};
        if (glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS)
            advance += camera->rotation * speed;
        if (glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS)
            advance -= camera->rotation * speed;
        if (glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS)
            advance -= camera->right * speed;
        if (glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS)
            advance += camera->right * speed;
        if (glfwGetKey(window->window, GLFW_KEY_SPACE) == GLFW_PRESS)
            advance.y += speed;
        if (glfwGetKey(window->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            advance.y -= speed;

        camera->position += advance;
    }
}