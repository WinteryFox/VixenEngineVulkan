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

        /**
         * Creates and allocates a vertex buffer for the mesh
         *
         * @param vertices The vertices of the mesh
         * @return Returns true on success
         */
        bool createVertexBuffer(const std::vector<glm::vec3> &vertices);
    };

    /**
     * Create and allocate a new buffer, it is up to the user to delete the buffer and free the device memory
     *
     * @param[in] logicalDevice The logical device to create this buffer on
     * @param[in] physicalDevice The physical device, required to get the best memory region
     * @param[in] size The size to allocate
     * @param[in] usage How this buffer will be used
     * @param[in] properties The memory properties required
     * @param[out] buffer The buffer to write to
     * @param[out] bufferMemory The buffer memory to allocate and write to
     * @return Returns true on successful creation and allocation of the buffer
     * @see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferUsageFlagBits.html
     */
    static bool createBuffer(const std::unique_ptr<LogicalDevice> &logicalDevice,
                             const std::unique_ptr<PhysicalDevice> &physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer,
                             VkDeviceMemory &bufferMemory) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice->device, &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
            error("Failed to create vertex buffer");
            return false;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(logicalDevice->device, buffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        physicalDevice->findMemoryType(
                memoryRequirements.memoryTypeBits,
                properties,
                allocateInfo.memoryTypeIndex
        );

        if (vkAllocateMemory(logicalDevice->device, &allocateInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            error("Failed to allocate buffer memory");
            vkDestroyBuffer(logicalDevice->device, buffer, nullptr);
            return false;
        }

        vkBindBufferMemory(logicalDevice->device, buffer, bufferMemory, 0);
        return true;
    }
}