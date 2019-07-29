#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include "LogicalDevice.h"
#include "Mesh.h"

namespace vixen {
    class Loader {
        /**
         * The command pool used for memory transfer operations
         */
        VkCommandPool transferCommandPool = VK_NULL_HANDLE;

        /**
         * The memory transfer fence
         */
        VkFence transferFence = VK_NULL_HANDLE;

        const std::unique_ptr<LogicalDevice> &logicalDevice;
        const std::unique_ptr<PhysicalDevice> &physicalDevice;

        /**
         * Copy a buffer from one location to another
         *
         * @param src The source buffer
         * @param dst The destination buffer
         * @param size The size of the source buffer
         */
        void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

        /**
         * Creates and allocates a vertex buffer for a mesh
         *
         * @param vertices The vertices of the mesh
         * @param vertexBuffer The vertex buffer to write to
         * @param vertexBufferMemory The vertex buffer memory to write to
         * @return Returns true on success
         */
        bool createVertexBuffer(const std::vector<glm::vec3> &vertices, VkBuffer &vertexBuffer,
                                VkDeviceMemory &vertexBufferMemory);

    public:
        Loader(const std::unique_ptr<LogicalDevice> &logicalDevice,
               const std::unique_ptr<PhysicalDevice> &physicalDevice);

        ~Loader();

        bool createMesh(const std::vector<glm::vec3> &vertices, std::unique_ptr<Mesh> &mesh);
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