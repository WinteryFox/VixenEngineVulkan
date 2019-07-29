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

        if (vkCreateFence(logicalDevice->device, &fenceCreateInfo, nullptr, &transferFence) != VK_SUCCESS) {
            fatal("Failed to create memory transfer fence");
        }
    }

    Loader::~Loader() {
        vkDeviceWaitIdle(logicalDevice->device);

        vkDestroyFence(logicalDevice->device, transferFence, nullptr);

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
                                    VkDeviceMemory &vertexBufferMemory) {
        VkDeviceSize bufferSize = sizeof(glm::vec3) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                     stagingBufferMemory);

        void *data;
        vkMapMemory(logicalDevice->device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(logicalDevice->device, stagingBufferMemory);

        createBuffer(logicalDevice, physicalDevice, bufferSize,
                     VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(logicalDevice->device, stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice->device, stagingBufferMemory, nullptr);

        return true;
    }

    bool Loader::createMesh(const std::vector<glm::vec3> &vertices, std::unique_ptr<Mesh> &mesh) {
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        if (!createVertexBuffer(vertices, vertexBuffer, vertexBufferMemory))
            return false;

        mesh = std::make_unique<Mesh>(logicalDevice, vertexBuffer, vertexBufferMemory, vertices.size());
        return true;
    }
}