#include "Mesh.h"

namespace vixen {
    Mesh::Mesh(const VkBuffer &buffer, const VmaAllocation &allocation, uint32_t vertexCount, uint32_t indexCount)
            : buffer(buffer), allocation(allocation), vertexCount(vertexCount),
              indexCount(indexCount) {
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;
    }
}