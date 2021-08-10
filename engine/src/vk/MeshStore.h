#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <memory>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "ImageView.h"

namespace Vixen::Vk {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;

        explicit MeshStore(std::shared_ptr<LogicalDevice> logicalDevice,
                           std::shared_ptr<PhysicalDevice> physicalDevice) : logicalDevice(std::move(logicalDevice)),
                                                                             physicalDevice(
                                                                                     std::move(physicalDevice)) {}

        void loadMesh(const std::string &path) {
            Assimp::Importer importer;
            const auto aiScene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                         aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
                                                         aiProcess_FlipUVs);
            if (!aiScene)
                throw std::runtime_error("Failed to open model");

            for (uint32_t i = 0; i < aiScene->mNumMeshes; i++) {
                const auto &aiMesh = aiScene->mMeshes[i];
                std::vector<glm::vec3> vertices;
                std::vector<uint32_t> indices;
                std::vector<glm::vec2> uvs;
                std::vector<glm::vec4> colors;

                vertices.reserve(aiMesh->mNumVertices);
                for (unsigned int j = 0; j < aiMesh->mNumVertices; j++) {
                    const auto &vertex = aiMesh->mVertices[j];
                    vertices.emplace_back(vertex.x, vertex.y, vertex.z);

                    if (aiMesh->HasTextureCoords(0)) {
                        const auto &uv = aiMesh->mTextureCoords[0][j];
                        uvs.emplace_back(uv.x, uv.y);
                    } else {
                        uvs.emplace_back(0.0f, 0.0f);
                    }

                    if (aiMesh->HasVertexColors(0)) {
                        const auto &color = aiMesh->mColors[0][j];
                        colors.emplace_back(color.r, color.g, color.b, color.a);
                    } else {
                        colors.emplace_back(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                }

                indices.reserve(aiMesh->mNumFaces * 3);
                for (unsigned int x = 0; x < aiMesh->mNumFaces; x++) {
                    const auto &face = aiMesh->mFaces[x];
                    if (face.mNumIndices != 3) {
                        logger.warning("Skipping non-triangulated face");
                        continue;
                    }

                    indices.push_back(face.mIndices[0]);
                    indices.push_back(face.mIndices[1]);
                    indices.push_back(face.mIndices[2]);
                }

                std::shared_ptr<ImageView> texture = nullptr;

                if (aiMesh->mMaterialIndex >= 0) {
                    const auto &material = aiScene->mMaterials[aiMesh->mMaterialIndex];
                    unsigned int count = material->GetTextureCount(aiTextureType_DIFFUSE);
                    for (unsigned int x = 0; x < count; x++) {
                        aiString str;
                        material->GetTexture(aiTextureType_DIFFUSE, x, &str);
                        std::string relative = str.C_Str();
                        std::replace(relative.begin(), relative.end(), '\\', '/');
                        try {
                            texture = std::make_shared<ImageView>(Image::from(logicalDevice,
                                                                              std::filesystem::path(
                                                                                      path).remove_filename().append(
                                                                                      relative).string()),
                                                                  VK_IMAGE_ASPECT_COLOR_BIT);
                        } catch (std::runtime_error &error) {
                            logger.warning("Failed to load texture at path \"{}\" ({})", relative, error.what());
                        }
                    }
                }

                const auto mesh = std::make_shared<Mesh>(
                        logicalDevice,
                        texture,
                        vertices,
                        indices,
                        uvs,
                        colors
                );
                meshes.push_back(mesh);
            }
        }

    private:
        Logger logger{"MeshStore"};
        const std::shared_ptr<LogicalDevice> logicalDevice;
        const std::shared_ptr<PhysicalDevice> physicalDevice;
    };
}