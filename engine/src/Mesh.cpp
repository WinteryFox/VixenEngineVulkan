#include "Mesh.h"

namespace Vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<glm::vec3> &vertices,
               const std::vector<uint32_t> &indices)
            : logicalDevice(logicalDevice), vertexCount(vertices.size()), indexCount(indices.size()) {
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;

        if (!createMeshBuffer(logicalDevice, vertices, indices, buffer, allocation)) {
            error("Failed to create vertex buffer");
        }
    }

    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &path) :
            logicalDevice(logicalDevice) {
        try {

        } catch (const std::runtime_error &e) {
            fatal("Failed to load model" + path);
        }

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;
        vertices.reserve(mesh->mNumVertices);
        vertices.reserve(mesh->mNumFaces * 3);

        for (size_t i = 0; i < mesh->mNumVertices; i++)
            vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        for (size_t i = 0; i < mesh->mNumFaces; i++) {
            indices.emplace_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[0]));
            indices.emplace_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[1]));
            indices.emplace_back(static_cast<uint32_t>(mesh->mFaces[i].mIndices[2]));
        }

        vertexCount = vertices.size();
        indexCount = indices.size();

        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;

        if (!createMeshBuffer(logicalDevice, vertices, indices, buffer, allocation)) {
            error("Failed to create vertex buffer");
        }
    }

    Mesh::~Mesh() {
        vmaDestroyBuffer(logicalDevice->allocator, buffer, allocation);
    }
}