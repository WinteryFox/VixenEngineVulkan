#pragma once

#include <utility>
#include "VulkanException.h"

namespace Vixen {
    struct ImageException : public VulkanException {
        const std::string imagePath;

        ImageException(VulkanException exception, std::string imagePath) : VulkanException(std::move(exception)), imagePath(std::move(imagePath)) {}
    };
}