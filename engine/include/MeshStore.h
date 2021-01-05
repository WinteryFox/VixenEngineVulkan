#pragma once

#include <memory>
#include <vector>
#include <memory>
#include <FBX/TriangulateProcess.h>
#include "Mesh.h"

namespace Vixen {
    struct MeshStore {
        std::vector<std::shared_ptr<Mesh>> meshes;

        explicit MeshStore(const std::unique_ptr<LogicalDevice> &logicalDevice) : logicalDevice(logicalDevice) {}

        void loadMesh(const std::string &path) {
            const auto &fbxScene = FBX::importFile("Fox.FBX", {new FBX::TriangulateProcess()});

            for (const auto &fbxMesh : fbxScene->models) {
                std::vector<glm::vec3> vertices;
                std::vector<uint32_t> indices;

                for (const auto &v : fbxMesh->mesh->vertices)
                    vertices.emplace_back(v.x, v.y, v.z);

                for (const auto &face : fbxMesh->mesh->faces) {
                    indices.push_back(face[0]);
                    indices.push_back(face[1]);
                    indices.push_back(face[2]);
                }

                const auto mesh = std::make_shared<Vixen::Mesh>(logicalDevice, vertices, indices);
                meshes.push_back(mesh);
            }
        }

    private:
        const std::unique_ptr<LogicalDevice> &logicalDevice;
    };
}