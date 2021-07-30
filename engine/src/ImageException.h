#pragma once

#include <utility>
#include "VulkanException.h"

namespace Vixen {
    struct ImageException : public VulkanException {
        explicit ImageException(VulkanException exception) : VulkanException(std::move(exception)) {}
    };
}