#pragma once

#include <vulkan/vulkan.h>
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include "Loader.h"

namespace Vixen {
    class Texture {
        const std::unique_ptr<LogicalDevice> &logicalDevice;
        VkImage texture = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;

    public:
        [[nodiscard]] VkImage getTexture() const;

        [[nodiscard]] VkImageView getView() const;

        [[nodiscard]] VmaAllocation getAllocation() const;

        Texture(const std::unique_ptr<LogicalDevice> &logicalDevice, const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::string &absolutePath);
        Texture &operator=(const Texture &texture) = delete;
        ~Texture();
    };
}