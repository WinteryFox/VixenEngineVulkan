#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <spdlog/spdlog.h>

#define VK_CHECK_RESULT(f) { \
    VkResult _result = (f); \
    if (_result != VK_SUCCESS) { \
        SPDLOG_ERROR("VkResult is \"{}\" ({})", Vixen::errorString(_result), _result); \
    } \
}

namespace Vixen {
    std::string errorString(VkResult result);

    void checkResult(VkResult result);
}
