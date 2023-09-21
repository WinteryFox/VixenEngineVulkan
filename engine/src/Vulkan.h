#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <string>
#include <spdlog/spdlog.h>

#define VK_CHECK_RESULT(f) { \
    VkResult _result = (f); \
    if (_result != VK_SUCCESS) { \
        SPDLOG_ERROR("VkResult is \"{}\"", string_VkResult(_result)); \
    } \
}

namespace Vixen {
    std::string errorString(VkResult result);
}
