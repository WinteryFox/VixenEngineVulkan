#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <FBX/FBXImport.h>
#include "LogicalDevice.h"
#include "Loader.h"
#include "Texture.h"

namespace Vixen {
    struct Mesh {
        /**
         * The amount of vertices this mesh consists of
         */
        const uint32_t vertexCount;

        /**
         * The amount of indices this mesh consists of
         */
        const uint32_t indexCount;

        const uint32_t uvCount;

        [[nodiscard]] VkBuffer getBuffer() const;

        [[nodiscard]] VmaAllocation getAllocation() const;

        const std::shared_ptr<Texture> texture;

        /**
         * Create a new mesh
         *
         * @param logicalDevice The device to be used to allocate the mesh for
         * @param vertexCount The amount of vertices this mesh has
         * @param indexCount The amount of indices this mesh has
         */
        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, std::shared_ptr<Texture> texture,
             const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices, const std::vector<glm::vec2> &uvs);

        Mesh(const Mesh &) = delete;

        ~Mesh();

        Mesh &operator=(const Mesh &mesh) = delete;

    private:
        /**
         * The vertex buffer for this mesh
         */
        VkBuffer buffer = VK_NULL_HANDLE;

        /**
         * The memory allocation for this mesh
         */
        VmaAllocation allocation = VK_NULL_HANDLE;

        const std::unique_ptr<LogicalDevice> &logicalDevice;
    };
}