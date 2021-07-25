#pragma once

#include <memory>
#include <vector>
#include <memory>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Logger.h"

namespace Vixen {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;

        explicit MeshStore(const std::unique_ptr<LogicalDevice> &logicalDevice,
                           const std::unique_ptr<PhysicalDevice> &physicalDevice) : logicalDevice(logicalDevice),
                                                                                    physicalDevice(physicalDevice) {}

        void loadMesh(const std::string &path) {
            try {
                Assimp::Importer importer;
                const auto aiScene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                             aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
                                                             aiProcess_FlipUVs);
                if (!aiScene) {
                    error(importer.GetErrorString());
                    return;
                }

                for (int i = 0; i < aiScene->mNumMeshes; i++) {
                    const auto aiMesh = aiScene->mMeshes[i];
                    std::vector<glm::vec3> vertices;
                    std::vector<uint32_t> indices;
                    std::vector<glm::vec2> uvs;

                    vertices.reserve(aiMesh->mNumVertices);
                    for (unsigned int x = 0; x < aiMesh->mNumVertices; x++) {
                        const auto &vertex = aiMesh->mVertices[x];
                        vertices.emplace_back(
                                vertex.x,
                                vertex.y,
                                vertex.z
                        );

                        if (aiMesh->mTextureCoords[0]) {
                            const auto uv = aiMesh->mTextureCoords[0][x];
                            uvs.emplace_back(uv.x, uv.y);
                        } else {
                            uvs.emplace_back(0.0f, 0.0f);
                        }
                    }

                    indices.reserve(aiMesh->mNumFaces * 3);
                    for (unsigned int x = 0; x < aiMesh->mNumFaces; x++) {
                        const auto &face = aiMesh->mFaces[x];
                        if (face.mNumIndices != 3) {
                            trace("Skipping non-triangulated face");
                            continue;
                        }

                        indices.push_back(face.mIndices[0]);
                        indices.push_back(face.mIndices[1]);
                        indices.push_back(face.mIndices[2]);
                    }

                    std::shared_ptr<Texture> texture = nullptr;

                    if (aiMesh->mMaterialIndex >= 0) {
                        const auto &material = aiScene->mMaterials[aiMesh->mMaterialIndex];
                        unsigned int count = material->GetTextureCount(aiTextureType_DIFFUSE);
                        for (unsigned int x = 0; x < count; x++) {
                            aiString str;
                            material->GetTexture(aiTextureType_DIFFUSE, x, &str);
                            std::string relative = str.C_Str();
                            std::replace(relative.begin(), relative.end(), '\\', '/');
                            texture = std::make_shared<Texture>(
                                    logicalDevice,
                                    physicalDevice,
                                    std::filesystem::path(path).remove_filename().append(relative).string()
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