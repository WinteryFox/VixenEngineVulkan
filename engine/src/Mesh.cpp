#include "Mesh.h"

namespace Vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<glm::vec3> &vertices,
               const std::vector<uint32_t> &indices)
            : logicalDevice(logicalDevice), vertexCount(vertices.size()), indexCount(indices.size()) {
        vertexCount = vertices.size();
        indexCount = indices.size();

        if (!createMeshBuffer(logicalDevice, vertices, indices, buffer, allocation)) {
            error("Failed to create vertex buffer");
        }
    }

    Mesh::~Mesh() {
        vmaDestroyBuffer(logicalDevice->allocator, buffer, allocation);
    }
}