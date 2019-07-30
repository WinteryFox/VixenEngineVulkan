#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "LogicalDevice.h"

namespace vixen {
    class Mesh {
    public:
        /**
         * The vertex buffer for this mesh
         */
        VkBuffer buffer;

        VmaAllocation allocation;

        /**
         * The amount of vertices this mesh consists of
         */
        const uint32_t vertexCount;

        const uint32_t indexCount;

        VkVertexInputBindingDescription bindingDescription = {};

        VkVertexInputAttributeDescription attributeDescription = {};

        /**
         * Create a new mesh
         *
         * @param buffer The buffer this mesh is allocated in
         * @param allocation The allocation this mesh is in
         * @param vertexCount The amount of vertices this mesh has
         * @param indexCount The amount of indices this mesh has
         */
        Mesh(const VkBuffer &buffer, const VmaAllocation &allocation, uint32_t vertexCount, uint32_t indexCount);

        Mesh(const Mesh &) = delete;

        Mesh &operator=(const Mesh &mesh) = delete;
    };
}