#include "Mesh.h"

#include <utility>

namespace vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice,
               const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::vector<glm::vec3> &vertices)
            : logicalDevice(logicalDevice), physicalDevice(physicalDevice), vertexCount(vertices.size()) {
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;

        createVertexBuffer(vertices, &vertexBuffer);
    }

    Mesh::~Mesh() {
        vkDestroyBuffer(logicalDevice->device, vertexBuffer, nullptr);
        vkFreeMemory(logicalDevice->device, vertexBufferMemory, nullptr);
    }

    bool Mesh::createVertexBuffer(const std::vector<glm::vec3> &vertices, VkBuffer *buffer) {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.size = sizeof(glm::vec3) * vertices.size();
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice->device, &bufferCreateInfo, nullptr, buffer) != VK_SUCCESS) {
            error("Failed to create vertex buffer");
            return false;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(logicalDevice->device, vertexBuffer, &memoryRequirements);

        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirements.size;
        physicalDevice->findMemoryType(
                memoryRequirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                allocateInfo.memoryTypeIndex
        );

        if (vkAllocateMemory(logicalDevice->device, &allocateInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
            error("Failed to allocate buffer memory");
            return false;
        }

        vkBindBufferMemory(logicalDevice->device, vertexBuffer, vertexBufferMemory, 0);

        void *data;
        vkMapMemory(logicalDevice->device, vertexBufferMemory, 0, bufferCreateInfo.size, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferCreateInfo.size);
        vkUnmapMemory(logicalDevice->device, vertexBufferMemory);

        return true;
    }
}