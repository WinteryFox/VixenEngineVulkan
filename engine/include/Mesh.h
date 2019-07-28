#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <memory>
#include "LogicalDevice.h"
#include "Logger.h"

namespace vixen {
    class Mesh {
    public:
        /**
         * The vertex buffer for this mesh
         */
        VkBuffer vertexBuffer = VK_NULL_HANDLE;

        /**
         * The memory allocated for the vertex buffer
         */
        VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;

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
        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::unique_ptr<PhysicalDevice> &physicalDevice,
             const std::vector<glm::vec3> &vertices);

        ~Mesh();

    private:
        const std::unique_ptr<LogicalDevice> &logicalDevice;
        const std::unique_ptr<PhysicalDevice> &physicalDevice;

        bool createVertexBuffer(const std::vector<glm::vec3> &vertices, VkBuffer *buffer);
    };
}