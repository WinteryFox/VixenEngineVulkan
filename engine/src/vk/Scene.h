#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"

namespace Vixen::Vk {
    struct Scene {
        Camera camera{};
        std::vector<Entity> entities;
    };
}