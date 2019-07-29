#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
#include <memory>
#include "Render.h"
#include "LogicalDevice.h"
#include "Mesh.h"

namespace vixen {
    /**
     * Loads, allocates and creates all buffers. Due to the way VMA allocates memory, the loader must exist
     * for the entire lifetime duration of all objects using it.
     */
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
         * Creates and allocates an index buffer for a mesh
         *
         * @param vertices The vertices of the mesh
         * @param indices The indices of the mesh
         * @param buffer The index buffer to write to
         * @param allocation The VMA allocation
         * @return Returns the buffer and allocation for the mesh
         */
        bool
        createMeshBuffer(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices, VkBuffer &buffer,
                         VmaAllocation &allocation);

        /**
         * Create and allocate a new buffer, it is up to the user to delete the buffer and free the device memory
         *
         * @param[in] logicalDevice The logical device to create this buffer on
         * @param[in] physicalDevice The physical device, required to get the best memory region
         * @param[in] size The size to allocate
         * @param[in] usage How this buffer will be used
         * @param[out] buffer The buffer to write to
         * @param[out] allocation The buffer memory allocated to by VMA
         * @return Returns true on successful creation and allocation of the buffer
         * @see https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkBufferUsageFlagBits.html
         */
        bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, VkBuffer &buffer,
                          VmaAllocation &allocation);

    public:
        Loader(const std::unique_ptr<LogicalDevice> &logicalDevice,
               const std::unique_ptr<PhysicalDevice> &physicalDevice);

        ~Loader();

        bool createMesh(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
                        std::unique_ptr<Mesh> &mesh);
    };
}