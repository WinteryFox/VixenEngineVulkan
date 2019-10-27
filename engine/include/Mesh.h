#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <FBX/FBXImport.hpp>
#include "LogicalDevice.h"
#include "Loader.h"

namespace Vixen {
    struct Mesh {
        /**
         * The vertex buffer for this mesh
         */
        VkBuffer buffer = VK_NULL_HANDLE;

        /**
         * The memory allocation for this mesh
         */
        VmaAllocation allocation = VK_NULL_HANDLE;

        /**
         * The amount of vertices this mesh consists of
         */
        uint32_t vertexCount;

        /**
         * The amount of indices this mesh consists of
         */
        uint32_t indexCount;

        VkVertexInputBindingDescription bindingDescription = {};

        VkVertexInputAttributeDescription attributeDescription = {};

        /**
         * Create a new mesh
         *
         * @param logicalDevice The device to be used to allocate the mesh for
         * @param vertexCount The amount of vertices this mesh has
         * @param indexCount The amount of indices this mesh has
         */
        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::vector<glm::vec3> &vertices,
             const std::vector<uint32_t> &indices);

        Mesh(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::string &path);

        Mesh(const Mesh &) = delete;

        ~Mesh();

        Mesh &operator=(const Mesh &mesh) = delete;

    private:
        const std::unique_ptr<LogicalDevice> &logicalDevice;
    };
}