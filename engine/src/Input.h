#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <map>
#include "Window.h"
#include "Camera.h"
#include "SimpleMath.h"

namespace Vixen {
    struct Input {
        const float speed = 1.38889f;

        const std::shared_ptr<Window> &window;

        explicit Input(const std::shared_ptr<Window> &window);

        //static void keyInputCallback(GLFWwindow* window, int key, int code, int action, int modifiers);

        void update(const std::unique_ptr<Camera> &camera, double deltaTime);

    private:
        double lastX = 0, lastY = 0;
    };
}