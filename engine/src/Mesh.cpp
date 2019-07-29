#include "Mesh.h"

namespace vixen {
    Mesh::Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const VkBuffer &vertexBuffer,
               const VkDeviceMemory &vertexBufferMemory, uint32_t vertexCount)
            : logicalDevice(logicalDevice), vertexBuffer(vertexBuffer), vertexBufferMemory(vertexBufferMemory),
              vertexCount(vertexCount) {
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        attributeDescription.binding = 0;
        attributeDescription.location = 0;
        attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescription.offset = 0;
    }

    Mesh::~Mesh() {
        vkDestroyBuffer(logicalDevice->device, vertexBuffer, nullptr);
        vkFreeMemory(logicalDevice->device, vertexBufferMemory, nullptr);
    }
}