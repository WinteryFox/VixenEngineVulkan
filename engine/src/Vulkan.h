#pragma once

#include <vulkan/vulkan.h>
#include <string>

#ifdef VIXEN_DEBUG
#include <spdlog/spdlog.h>

#define VK_CHECK_RESULT(f) { \
    VkResult _result = (f); \
    if (_result != VK_SUCCESS) { \
        SPDLOG_ERROR("VkResult is \"{}\" ({})", Vixen::errorString(_result), _result); \
    } \
}
#else
#define VK_CHECK_RESULT(f) (f);
#endif

namespace Vixen {
    std::string errorString(VkResult result);
}
