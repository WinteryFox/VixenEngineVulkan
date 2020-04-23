#include "Mesh.h"

#include <utility>

namespace Vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, std::shared_ptr<const Texture> texture,
               const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
               const std::vector<glm::vec2> &uvs)
            : logicalDevice(logicalDevice), vertexCount(vertices.size()), indexCount(indices.size()),
              uvCount(uvs.size()), texture(std::move(texture)) {
        VkDeviceSize vertexBufferSize = sizeof(glm::vec3) * vertices.size();
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
        VkDeviceSize uvBufferSize = sizeof(glm::vec2) * uvs.size();

        VkBuffer stagingBuffer;
        VmaAllocation stagingAllocation;
        createBuffer(logicalDevice, vertexBufferSize + indexBufferSize + uvBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VMA_MEMORY_USAGE_CPU_TO_GPU,
                     stagingBuffer, stagingAllocation);

        void *data;
        vmaMapMemory(logicalDevice->allocator, stagingAllocation, &data);
        memcpy(data, vertices.data(), (size_t) vertexBufferSize);
        memcpy(static_cast<char *>(data) + static_cast<size_t>(vertexBufferSize), indices.data(),
               (size_t) indexBufferSize);
        memcpy(static_cast<char *>(data) + static_cast<size_t>(vertexBufferSize) + static_cast<size_t>(indexBufferSize),
               uvs.data(), (size_t) uvBufferSize);
        vmaUnmapMemory(logicalDevice->allocator, stagingAllocation);

        if (!createBuffer(logicalDevice, vertexBufferSize + indexBufferSize + uvBufferSize,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, buffer, allocation))
            error("Failed to create buffer for vertices");

        copyBuffer(logicalDevice, stagingBuffer, buffer, vertexBufferSize + indexBufferSize + uvBufferSize);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);
    }

    Mesh::~Mesh() {
        vmaDestroyBuffer(logicalDevice->allocator, buffer, allocation);
    }

    VkBuffer Mesh::getBuffer() const {
        return buffer;
    }

    VmaAllocation Mesh::getAllocation() const {
        return allocation;
    }
}