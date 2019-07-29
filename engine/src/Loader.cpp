#include "Loader.h"

namespace vixen {
    Loader::Loader(const std::unique_ptr<LogicalDevice> &logicalDevice,
                   const std::unique_ptr<PhysicalDevice> &physicalDevice) : logicalDevice(logicalDevice),
                                                                            physicalDevice(physicalDevice) {
        VkCommandPoolCreateInfo transferCommandPoolCreateInfo = {};
        transferCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        transferCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        transferCommandPoolCreateInfo.queueFamilyIndex = physicalDevice->transferFamilyIndex;

        if (vkCreateCommandPool(logicalDevice->device, &transferCommandPoolCreateInfo, nullptr, &transferCommandPool) !=
            VK_SUCCESS)
            fatal("Failed to create memory transfer command pool");
        trace("Successfully created memory transfer command pool");

        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateFence(logicalDevice->device, &fenceCreateInfo, nullptr, &transferFence) != VK_SUCCESS)
            fatal("Failed to create memory transfer fence");

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.device = logicalDevice->device;
        allocatorCreateInfo.physicalDevice = physicalDevice->device;

        if (vmaCreateAllocator(&allocatorCreateInfo, &logicalDevice->allocator) != VK_SUCCESS)
            fatal("Failed to create VMA allocator");
    }

    Loader::~Loader() {
        vkDeviceWaitIdle(logicalDevice->device);

        vkDestroyFence(logicalDevice->device, transferFence, nullptr);

        vmaDestroyAllocator(logicalDevice->allocator);
        vkDestroyCommandPool(logicalDevice->device, transferCommandPool, nullptr);
    }

    void Loader::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandPool = transferCommandPool;
        allocateInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(logicalDevice->device, &allocateInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkResetFences(logicalDevice->device, 1, &transferFence);

        vkQueueSubmit(logicalDevice->transferQueue, 1, &submitInfo, transferFence);
        vkWaitForFences(logicalDevice->device, 1, &transferFence, true, std::numeric_limits<std::uint64_t>::max());

        vkFreeCommandBuffers(logicalDevice->device, transferCommandPool, 1, &commandBuffer);
    }

    bool Loader::createVertexBuffer(const std::vector<glm::vec3> &vertices, VkBuffer &vertexBuffer,
                                    VmaAllocation &vertexBufferAllocation) {
        VkDeviceSize bufferSize = sizeof(glm::vec3) * vertices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer,
                     stagingAllocation);

        void *data;
        vmaMapMemory(logicalDevice->allocator, stagingAllocation, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vmaUnmapMemory(logicalDevice->allocator, stagingAllocation);

        if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                          VMA_MEMORY_USAGE_CPU_TO_GPU, vertexBuffer, vertexBufferAllocation)) {
            error("Failed to create buffer for vertices");
            return false;
        }

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);

        return true;
    }

    bool Loader::createIndexBuffer(const std::vector<uint32_t> &indices, VkBuffer &indexBuffer,
                                   VmaAllocation &indexBufferAllocation) {
        VkDeviceSize bufferSize = sizeof(uint32_t) * indices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;
        if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY, stagingBuffer,
                          stagingAllocation)) {
            error("Failed to create buffer for indices");
            return false;
        }

        void *data;
        vmaMapMemory(logicalDevice->allocator, stagingAllocation, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
        vmaUnmapMemory(logicalDevice->allocator, stagingAllocation);

        if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                          VMA_MEMORY_USAGE_CPU_TO_GPU, indexBuffer, indexBufferAllocation)) {
            error("Failed to create buffer for vertices");
            return false;
        }

        copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        vmaDestroyBuffer(logicalDevice->allocator, stagingBuffer, stagingAllocation);

        return true;
    }

    bool Loader::createMesh(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
                            std::unique_ptr<Mesh> &mesh) {
        VkBuffer vertexBuffer = VK_NULL_HANDLE;
        VmaAllocation vertexBufferAllocation = VK_NULL_HANDLE;

        VkBuffer indexBuffer = VK_NULL_HANDLE;
        VmaAllocation indexBufferAllocation = VK_NULL_HANDLE;

        if (!createVertexBuffer(vertices, vertexBuffer, vertexBufferAllocation)) {
            error("Failed to create vertex buffer");
            return false;
        }

        if (!createIndexBuffer(indices, indexBuffer, indexBufferAllocation)) {
            error("Failed to create indices buffer");
            return false;
        }

        mesh = std::make_unique<Mesh>(logicalDevice, vertexBuffer, vertexBufferAllocation, indexBuffer,
                                      indexBufferAllocation, vertices.size(), indices.size());
        return true;
    }

    bool Loader::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage, VkBuffer &buffer,
                              VmaAllocation &allocation) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocationCreateInfo = {};
        allocationCreateInfo.usage = vmaUsage;

        if (vmaCreateBuffer(logicalDevice->allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation,
                            nullptr) != VK_SUCCESS) {
            error("Failed to allocate buffer memory");
            vkDestroyBuffer(logicalDevice->device, buffer, nullptr);
            return false;
        }

        return true;
    }
}