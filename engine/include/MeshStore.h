#pragma once

#include <memory>
#include <vector>
#include <memory>
#include "Mesh.h"

namespace Vixen {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;

        explicit MeshStore(const std::unique_ptr<LogicalDevice> &logicalDevice,
                           const std::unique_ptr<PhysicalDevice> &physicalDevice) : logicalDevice(logicalDevice),
                                                                                    physicalDevice(physicalDevice) {}

        void loadMesh(const std::string &path) {
            const auto &fbxScene = FBX::importFile(path, FBX::Process::TRIANGULATE);

            for (const auto &fbxModel : fbxScene->models) {
                std::vector<glm::vec3> vertices;
                std::vector<uint32_t> indices;
                std::vector<glm::vec2> uvs;

                for (const auto &v : fbxModel->mesh->vertices)
                    vertices.emplace_back(v.x, v.y, v.z);

                for (const auto &face : fbxModel->mesh->faces) {
                    if (face.indices.size() != 3) {
                        warning("Skipping non-triangulated face (" + std::to_string(face.indices.size()) +
                                ") in model \"" + path + "\"");
                        continue;
                    }

                    indices.push_back(face[0]);
                    indices.push_back(face[1]);
                    indices.push_back(face[2]);
                }

                for (const auto &uv : fbxModel->mesh->uvs)
                    uvs.emplace_back(uv.x, 1.0f - uv.y);

                const auto mesh = std::make_shared<Mesh>(
                        logicalDevice,
                        std::make_shared<Texture>(
                                logicalDevice,
                                physicalDevice,
                                fbxModel->material->texture->relativePath
                        ),
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