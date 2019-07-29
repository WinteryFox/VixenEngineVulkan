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

        createVertexBuffer(vertices);
    }

    Mesh::~Mesh() {
        vkDestroyBuffer(logicalDevice->device, vertexBuffer, nullptr);
        vkFreeMemory(logicalDevice->device, vertexBufferMemory, nullptr);
    }

    bool Mesh::createVertexBuffer(const std::vector<glm::vec3> &vertices) {
        VkDeviceSize bufferSize = sizeof(glm::vec3) * vertices.size();

        createBuffer(logicalDevice, physicalDevice, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer,
                     vertexBufferMemory);

        void *data;
        vkMapMemory(logicalDevice->device, vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(logicalDevice->device, vertexBufferMemory);
        return true;
    }
}