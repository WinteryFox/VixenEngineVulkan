#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Vulkan.h"
#include "LogicalDevice.h"
#include "Texture.h"
#include "Buffer.h"

namespace Vixen {
    class Mesh {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        std::unique_ptr<Buffer> buffer;

    public:
        /**
         * The amount of vertices this mesh consists of
         */
        const uint32_t vertexCount;

        /**
         * The amount of indices this mesh consists of
         */
        const uint32_t indexCount;

        [[nodiscard]] const std::unique_ptr<Buffer> &getBuffer() const;

        const std::shared_ptr<const Texture> texture;

        /**
         * Create a new mesh
         *
         * @param logicalDevice The device to be used to allocate the mesh for
         * @param vertexCount The amount of vertices this mesh has
         * @param indexCount The amount of indices this mesh has
         */
        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, std::shared_ptr<const Texture> texture,
             const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
             const std::vector<glm::vec2> &uvs, const std::vector<glm::vec4> &colors);

        Mesh(const Mesh &) = delete;

        Mesh &operator=(const Mesh &mesh) = delete;
    };
}