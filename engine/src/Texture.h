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
        std::shared_ptr<Image> image;

        std::unique_ptr<ImageView> view;

    public:
        Texture(const std::unique_ptr<LogicalDevice> &logicalDevice,
                const std::unique_ptr<PhysicalDevice> &physicalDevice, const std::string &path);

        [[nodiscard]] const std::shared_ptr<Image> &getImage() const;

        [[nodiscard]] const std::unique_ptr<ImageView> &getView() const;
    };
}