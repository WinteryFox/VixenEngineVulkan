#include "Mesh.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
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
        Assimp::Importer importer = Assimp::Importer();
        const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
                                                       aiProcess_PreTransformVertices | aiProcess_CalcTangentSpace |
                                                       aiProcess_Triangulate);
        if (!scene)
            fatal("Failed to load mesh " + path);
        aiMesh *mesh = scene->mMeshes[0];

        std::vector<glm::vec3> vertices;
        std::vector<uint32_t> indices;

        for (size_t i = 0; i < mesh->mNumVertices; i++)
            vertices.emplace_back(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        for (size_t i = 0; i < mesh->mNumFaces; i++) {
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[1]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
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
#pragma clang diagnostic pop