#pragma once

#include <memory>
#include <vector>
#include "Mesh.h"
#include "Entity.h"

namespace Vixen {
    struct Scene {
        std::vector<Entity> entities;
    };
}