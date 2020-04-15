#pragma once

#include <memory>
#include <vector>
#include <memory>
#include "Mesh.h"

namespace Vixen {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Texture>> textures;

        explicit MeshStore(const std::unique_ptr<LogicalDevice> &logicalDevice,
                           const std::unique_ptr<PhysicalDevice> &physicalDevice) : logicalDevice(logicalDevice),
                                                                                    physicalDevice(physicalDevice) {}

        void loadMesh(const std::string &path) {
            const auto &fbxScene = FBX::importFile(path, 0);

            for (const auto &fbxModel : fbxScene->models) {
                std::vector<glm::vec3> vertices;
                std::vector<uint32_t> indices;
                std::vector<glm::vec2> uvs;

                for (const auto &v : fbxModel->mesh->vertices)
                    vertices.emplace_back(v.x, v.y, v.z);

                for (const auto &face : fbxModel->mesh->faces) {
                    if (face.indices.size() != 3)
                        throw std::runtime_error("Face does not have 3 indices, model is not triangulated.");

                    indices.push_back(face[0]);
                    indices.push_back(face[1]);
                    indices.push_back(face[2]);
                }

                for (const auto &uv : fbxModel->mesh->uvs)
                    uvs.emplace_back(uv.x, uv.y);

                const auto texture = std::make_shared<Texture>(
                        logicalDevice,
                        physicalDevice,
                        fbxModel->material->texture->relativePath
                );
                textures.push_back(texture);
                const auto mesh = std::make_shared<Mesh>(
                        logicalDevice,
                        texture,
                        vertices,
                        indices,
                        uvs
                );
                meshes.push_back(mesh);
            }
        }

    private:
        const std::unique_ptr<LogicalDevice> &logicalDevice;
        const std::unique_ptr<PhysicalDevice> &physicalDevice;
    };
}