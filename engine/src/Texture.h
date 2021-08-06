#pragma once

#include <vulkan/vulkan.h>
#include <stb_image.h>
#include <string>
#include <stdexcept>
#include <memory>
#include "ImageView.h"
#include "Buffer.h"

namespace Vixen {
    class Texture {
        std::shared_ptr<ImageView> image;

    public:
        Texture(const std::shared_ptr<LogicalDevice> &logicalDevice,
                const std::shared_ptr<PhysicalDevice> &physicalDevice, const std::string &path);

        [[nodiscard]] const std::shared_ptr<ImageView> &getImage() const;
    };
}