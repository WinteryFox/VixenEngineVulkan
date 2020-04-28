#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace Vixen {
    struct VulkanException : public std::runtime_error {
        const VkResult result;

        VulkanException(const std::string &message, VkResult result) : std::runtime_error(message), result(result) {}
    };
}