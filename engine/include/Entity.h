#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <memory>
#include "Mesh.h"

namespace Vixen {
    struct Entity {
        glm::vec3 position{};
        glm::vec3 rotation{};
        float scale = 1.0f;

        const std::shared_ptr<Mesh> &mesh;

        explicit Entity(const std::shared_ptr<Mesh> &mesh) : mesh(mesh) {};

        glm::mat4 getModelMatrix() {
            return glm::mat4(glm::translate(glm::mat4(1.0f) * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        }
    };
}