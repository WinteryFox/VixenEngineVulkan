#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"

namespace Vixen {
    struct Scene {
        Camera camera{};
        std::vector<Entity> entities;
    };
}