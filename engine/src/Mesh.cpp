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
        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        try {
            const auto &mesh = FBX::importFile(path, FBX::Process::TRIANGULATE);

            vertices.reserve(mesh.vertices.size());
            vertices.reserve(mesh.faces.size() * 3);

            for (const auto &vertex : mesh.vertices)
                vertices.emplace_back(vertex.x, vertex.y, vertex.z);

            for (const auto &face : mesh.faces) {
                indices.push_back(face.indices[0]);
                indices.push_back(face.indices[1]);
                indices.push_back(face.indices[2]);
            }
        } catch (const std::runtime_error &e) {
            fatal("Failed to load model " + path);
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