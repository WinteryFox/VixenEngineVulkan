#include "Vulkan.h"
#include "LogicalDevice.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include <vk_mem_alloc.h>

namespace Vixen {
    class Image {
        const std::shared_ptr<LogicalDevice> device;

        VkImage image{};

        VmaAllocation allocation{};

        uint16_t width;

        uint16_t height;

        VkFormat format;

        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageUsageFlags usage;

    public:
        Image(const std::shared_ptr<LogicalDevice> &device, uint16_t width, uint16_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

        Image(const Image &other) = delete;

        ~Image();

        void transition(VkImageLayout newLayout);

        void copyFrom(const Buffer &buffer);

        [[nodiscard]] std::shared_ptr<LogicalDevice> getDevice() const;

        [[nodiscard]] VkImage getImage() const;

        [[nodiscard]] VmaAllocation getAllocation() const;

        [[nodiscard]] VkFormat getFormat() const;

        [[nodiscard]] VkImageUsageFlags getUsage() const;
    };
}
