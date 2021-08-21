#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace Vixen {
    struct VulkanException : public std::runtime_error {
        explicit VulkanException(const std::string &message) : std::runtime_error(message) {}
    };
}