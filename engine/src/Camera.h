#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vixen {
    struct Camera {
        const float fieldOfView;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 right{1.0f, 0.0f, 0.0f};
        const float nearPlane, farPlane;
        double horizontal = 3.14, vertical = 0.0;

        explicit Camera(const glm::vec3 &position = {}, const glm::vec3 &rotation = {}, const float fieldOfView = 45.0f,
                        const float nearPlane = 0.01f, const float farPlane = 1000.0f) :
                fieldOfView(fieldOfView), position(position), rotation(rotation), nearPlane(nearPlane),
                farPlane(farPlane) {}

        [[nodiscard]] glm::mat4 getView() const {
            return glm::lookAt(position, position + rotation, {0.0f, 1.0f, 0.0f});
        }

        [[nodiscard]] glm::mat4 getProjection(const float aspectRatio) const {
            return glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
        }
    };
}