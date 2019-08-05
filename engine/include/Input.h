#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "Window.h"
#include "Camera.h"

namespace Vixen {
    struct Input {
        const float speed = 0.0005f;

        const std::unique_ptr<Window> &window;

        explicit Input(const std::unique_ptr<Window> &window);

        //static void keyInputCallback(GLFWwindow* window, int key, int code, int action, int modifiers);

        void update(const std::unique_ptr<Camera> &camera);

    private:
        double horizontal = 0.0, vertical = 0.0;
        double lastX = 0, lastY = 0;
    };
}