#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "Entity.h"

namespace vixen {
    struct Scene {
        std::vector<Entity> entities;
    };
}