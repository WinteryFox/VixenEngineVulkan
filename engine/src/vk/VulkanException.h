#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

namespace Vixen::Vk {
    struct VulkanException : public std::runtime_error {
        explicit VulkanException(const std::string &message) : std::runtime_error(message) {}
    };
}