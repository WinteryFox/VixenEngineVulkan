#pragma once

#include <vulkan/vulkan.h>
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include "IOException.h"
#include "Vulkan.h"

namespace Vixen {
    class Texture {
        const std::unique_ptr<LogicalDevice> &logicalDevice;

        VkImage texture;

        VkImageView view;

        VmaAllocation allocation;

    public:
        Texture(const std::unique_ptr<LogicalDevice> &logicalDevice,
                const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::string &path);

        Texture &operator=(const Texture &texture) = delete;

        ~Texture();

        [[nodiscard]] VkImageView getView() const;
    };
}