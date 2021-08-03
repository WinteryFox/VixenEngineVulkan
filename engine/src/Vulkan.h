#pragma once

#include <vulkan/vulkan.h>

#ifdef VIXEN_DEBUG
#include <spdlog/spdlog.h>

#define VK_CHECK_RESULT(f) { \
    VkResult result = (f); \
    if (result != VK_SUCCESS) { \
        SPDLOG_ERROR("VkResult is \"{}\" ({}) at %g:%#", Vixen::errorString(result), result); \
    } \
}
#else
#define VK_CHECK_RESULT(f) (f);
#endif

namespace Vixen {
    std::string errorString(VkResult result);
}
