#include "Vulkan.h"
#include "LogicalDevice.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include <vk_mem_alloc.h>

namespace Vixen {
    class Image {
        const std::unique_ptr<LogicalDevice> &device;

        VkImage image{};

        VmaAllocation allocation{};

        uint16_t width;

        uint16_t height;

        VkFormat format;

        VkImageLayout layout;

        VkImageUsageFlags usage;

    public:
        Image(const std::unique_ptr<LogicalDevice> &device, uint16_t width, uint16_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);

        Image(const Image &other) = delete;

        ~Image();

        void transition(VkImageLayout newLayout);

        void copyFrom(const Buffer &buffer);

        [[nodiscard]] const std::unique_ptr<LogicalDevice> &getDevice() const;

        [[nodiscard]] VkImage getImage() const;

        [[nodiscard]] VmaAllocation getAllocation() const;

        [[nodiscard]] VkFormat getFormat() const;

        [[nodiscard]] VkImageUsageFlags getUsage() const;
    };
}
