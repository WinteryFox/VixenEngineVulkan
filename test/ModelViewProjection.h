#pragma once

namespace Vixen::Test {
    struct ModelViewProjection {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };
}