#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace Vixen {
    struct VulkanException : public std::runtime_error {
        const VkResult result;

        VulkanException(VkResult result, const std::string &message) : std::runtime_error(message), result(result) {}
    };
}