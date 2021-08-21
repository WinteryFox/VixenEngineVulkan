#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "LogicalDevice.h"
#include "Buffer.h"
#include "ImageView.h"

namespace Vixen {
    class Mesh {
        const std::shared_ptr<LogicalDevice> logicalDevice;

        std::unique_ptr<Buffer> buffer;

        const uint32_t vertexCount;

        const uint32_t indexCount;

        const std::shared_ptr<const ImageView> texture;

    public:
        Mesh(const std::shared_ptr<LogicalDevice> &logicalDevice, const std::shared_ptr<ImageView> &texture,
             const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
             const std::vector<glm::vec2> &uvs, const std::vector<glm::vec4> &colors);

        Mesh(const Mesh &) = delete;

        Mesh &operator=(const Mesh &) = delete;

        [[nodiscard]] const std::unique_ptr<Buffer> &getBuffer() const;

        [[nodiscard]] uint32_t getVertexCount() const;

        [[nodiscard]] uint32_t getIndexCount() const;

        [[nodiscard]] const std::shared_ptr<const ImageView> &getTexture() const;
    };
}