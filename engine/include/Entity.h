#pragma once

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>
#include "Mesh.h"
#include "SimpleMath.h"

namespace Vixen {
    struct Entity {
        glm::vec3 position;
        glm::vec3 rotation;
        float scale;

        const std::shared_ptr<Mesh> &mesh;

        explicit Entity(const std::shared_ptr<Mesh> &mesh, glm::vec3 position = {}, glm::vec3 rotation = {},
                        float scale = 1.0f) : mesh(mesh), position(position), rotation(rotation), scale(scale) {};

        [[nodiscard]] glm::mat4 getModelMatrix() const {
            return glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
                   glm::toMat4(glm::quat(rotation * static_cast<float>(PI / 180.0))) *
                   glm::translate(glm::mat4(1.0f), position);
        }
    };
}