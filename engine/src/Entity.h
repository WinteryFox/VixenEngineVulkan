#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include <numbers>
#include "Mesh.h"
#include "SimpleMath.h"

namespace Vixen {
    struct Entity {
        const std::shared_ptr<Mesh> &mesh;
        glm::vec3 position;
        glm::vec3 rotation;
        float scale;

        explicit Entity(const std::shared_ptr<Mesh> &mesh, glm::vec3 position = {}, glm::vec3 rotation = {},
                        float scale = 1.0f) : mesh(mesh), position(position), rotation(rotation), scale(scale) {};

        [[nodiscard]] glm::mat4 getModelMatrix() const {
            return glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
                   glm::toMat4(glm::quat(rotation * std::numbers::pi_v<float> / 180.0f)) *
                   glm::translate(glm::mat4(1.0f), position);
        }
    };
}