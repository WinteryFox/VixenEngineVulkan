#pragma once

#include <memory>
#include <vector>
#include <memory>
#include <FBX/FBXImport.h>
#include <FBX/TriangulateProcess.h>
#include <filesystem>
#include "Mesh.h"

namespace Vixen {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;

        explicit MeshStore(const std::unique_ptr<LogicalDevice> &logicalDevice,
                           const std::unique_ptr<PhysicalDevice> &physicalDevice) : logicalDevice(logicalDevice),
                                                                                    physicalDevice(physicalDevice) {}

        void loadMesh(const std::string &path) {
            try {
                const auto &fbxScene = FBX::importFile(path, std::set<FBX::Process *>{new FBX::TriangulateProcess()});

                for (const auto &fbxModel : fbxScene->models) {
                    std::vector<glm::vec3> vertices;
                    std::vector<uint32_t> indices;
                    std::vector<glm::vec2> uvs;

                    vertices.reserve(fbxModel->mesh->vertices.size());
                    for (const auto &v : fbxModel->mesh->vertices)
                        vertices.emplace_back(v.x, v.y, v.z);

                    indices.reserve(fbxModel->mesh->indexCount);
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

                    uvs.reserve(fbxModel->mesh->uvs.size());
                    for (const auto &uv : fbxModel->mesh->uvs)
                        uvs.emplace_back(uv.x, 1.0f - uv.y);

                    std::shared_ptr<Texture> texture = nullptr;

                    if (fbxModel->material != nullptr && fbxModel->material->texture != nullptr) {
                        try {
                            texture = std::make_shared<Texture>(
                                    logicalDevice,
                                    physicalDevice,
                                    fbxModel->material->texture->relativePath
                            );
                        } catch (std::runtime_error &ignored) {
                            texture = std::make_shared<Texture>(
                                    logicalDevice,
                                    physicalDevice,
                                    fbxModel->material->texture->absolutePath
                            );
                        }
                    }

                    const auto mesh = std::make_shared<Mesh>(
                            logicalDevice,
                            texture,
                            vertices,
                            indices,
                            uvs
                    );
                    meshes.push_back(mesh);
                }
            } catch (const IOException &exception) {
                throw exception;
            } catch (const std::runtime_error &exception) {
                throw IOException("Failed to open model", path);
            }
        }

    private:
        const std::unique_ptr<LogicalDevice> &logicalDevice;
        const std::unique_ptr<PhysicalDevice> &physicalDevice;
    };
}