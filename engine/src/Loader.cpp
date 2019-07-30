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

        if (vmaCreateAllocator(&allocatorCreateInfo, &allocator) != VK_SUCCESS)
            fatal("Failed to create VMA allocator");
    }

    Loader::~Loader() {
        vkDeviceWaitIdle(logicalDevice->device);

        for (auto &mesh : meshes)
            vmaDestroyBuffer(allocator, mesh->buffer, mesh->allocation);

        vkDestroyFence(logicalDevice->device, transferFence, nullptr);

        vkDestroyCommandPool(logicalDevice->device, transferCommandPool, nullptr);
        vmaDestroyAllocator(allocator);
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

    bool Loader::createMeshBuffer(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
                                  VkBuffer &buffer, VmaAllocation &allocation) {
        VkDeviceSize vertexBufferSize = sizeof(glm::vec3) * vertices.size();
        VkDeviceSize indexBufferSize = sizeof(uint32_t) * indices.size();

        VkBuffer stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAllocation = VK_NULL_HANDLE;
        createBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU,
                     stagingBuffer, stagingAllocation);

        void *data;
        vmaMapMemory(allocator, stagingAllocation, &data);
        memcpy(data, vertices.data(), (size_t) vertexBufferSize);
        memcpy(static_cast<char *>(data) + static_cast<size_t>(vertexBufferSize), indices.data(),
               (size_t) indexBufferSize);
        vmaUnmapMemory(allocator, stagingAllocation);

        if (!createBuffer(vertexBufferSize + indexBufferSize,
                          VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                          VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY, buffer, allocation)) {
            error("Failed to create buffer for vertices");
            return false;
        }

        copyBuffer(stagingBuffer, buffer, vertexBufferSize + indexBufferSize);

        vmaDestroyBuffer(allocator, stagingBuffer, stagingAllocation);

        return true;
    }

    bool Loader::createMesh(const std::vector<glm::vec3> &vertices, const std::vector<uint32_t> &indices,
                            std::shared_ptr<Mesh> &mesh) {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;

        if (!createMeshBuffer(vertices, indices, buffer, allocation)) {
            error("Failed to create vertex buffer");
            return false;
        }

        meshes.push_back(std::make_shared<Mesh>(buffer, allocation, vertices.size(), indices.size()));
        mesh = meshes[0];
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

        if (vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &buffer, &allocation,
                            nullptr) != VK_SUCCESS) {
            error("Failed to allocate buffer memory");
            return false;
        }

        return true;
    }
}