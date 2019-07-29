#include "Mesh.h"

namespace vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &vertexBuffer,
               const VmaAllocation &vertexBufferAllocation, const VkBuffer &indexBuffer,
               const VmaAllocation &indexBufferAllocation, uint32_t vertexCount, uint32_t indexCount)
            : logicalDevice(logicalDevice), vertexBuffer(vertexBuffer), vertexBufferAllocation(vertexBufferAllocation),
              indexBuffer(indexBuffer), indexBufferAllocation(indexBufferAllocation), vertexCount(vertexCount),
              indexCount(indexCount) {
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;
    }

    Mesh::~Mesh() {
        vmaDestroyBuffer(logicalDevice->allocator, vertexBuffer, vertexBufferAllocation);
        vmaDestroyBuffer(logicalDevice->allocator, indexBuffer, indexBufferAllocation);
    }
}