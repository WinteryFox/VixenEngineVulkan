#pragma once

#include <memory>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "LogicalDevice.h"

namespace vixen {
    class Mesh {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

    public:
        /**
         * The vertex buffer for this mesh
         */
        VkBuffer vertexBuffer;

        /**
         * The memory allocated for the vertex buffer
         */
        VkDeviceMemory vertexBufferMemory;

        /**
         * The amount of vertices this mesh consists of
         */
        const uint32_t vertexCount;

        VkVertexInputBindingDescription bindingDescription = {};

        VkVertexInputAttributeDescription attributeDescription = {};

        /**
         * Create a new mesh
         *
         * @param logicalDevice The logical device to be used when creating the buffers, will retain a reference to
         * destroy the buffers
         * @param vertices The vertices for this mesh
         */
        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &vertexBuffer,
             const VkDeviceMemory &vertexBufferMemory, uint32_t vertexCount);

        ~Mesh();
    };
}