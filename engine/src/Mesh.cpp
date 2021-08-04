#include "Mesh.h"

namespace Vixen {
    Mesh::Mesh(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::shared_ptr<Texture> &texture,
               const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
               const std::vector<glm::vec2> &uvs, const std::vector<glm::vec4> &colors)
            : logicalDevice(logicalDevice), vertexCount(vertices.size()), indexCount(indices.size()),
              texture(texture) {
        if (vertices.size() != uvs.size())
            throw std::runtime_error("Vertex count must be equal to UV count");
        if (vertices.size() != colors.size())
            throw std::runtime_error("Vertex count must be equal to color count");

        VkDeviceSize vertexBufferSize = sizeof(glm::vec3) * vertices.size();
        VkDeviceSize uvBufferSize = sizeof(glm::vec2) * vertices.size();
        VkDeviceSize colorBufferSize = sizeof(glm::vec4) * vertices.size();
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();
        VkDeviceSize size = vertexBufferSize + uvBufferSize + colorBufferSize + indexBufferSize;
        auto stagingBuffer = Buffer(logicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VMA_MEMORY_USAGE_CPU_TO_GPU);

        void* data = stagingBuffer.map();
        memcpy(data, vertices.data(), vertexBufferSize);
        memcpy(static_cast<char *>(data) + vertexBufferSize, uvs.data(), uvBufferSize);
        memcpy(static_cast<char *>(data) + vertexBufferSize + uvBufferSize, colors.data(), colorBufferSize);
        memcpy(static_cast<char *>(data) + vertexBufferSize + uvBufferSize + colorBufferSize, indices.data(),
               indexBufferSize);
        stagingBuffer.unmap();

        buffer = std::make_unique<Buffer>(logicalDevice, size,
                                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
        buffer->copyFrom(stagingBuffer);
    }

    const std::unique_ptr<Buffer> &Mesh::getBuffer() const {
        return buffer;
    }

    uint32_t Mesh::getVertexCount() const {
        return vertexCount;
    }

    uint32_t Mesh::getIndexCount() const {
        return indexCount;
    }

    const std::shared_ptr<const Texture> &Mesh::getTexture() const {
        return texture;
    }
}