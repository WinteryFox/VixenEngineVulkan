#include "Vulkan.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "LogicalDevice.h"

namespace Vixen {
    class Image {
        VmaAllocation allocation{};

        uint32_t width;

        uint32_t height;

        VkFormat format;

        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

        VkImageUsageFlags usageFlags;

    protected:
        const std::shared_ptr<LogicalDevice> device;

        VkImage image{};

    public:
        Image(const std::shared_ptr<LogicalDevice> &device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags);

        Image(VkImage image);

        Image(const Image &other) = delete;

        ~Image();

        void transition(VkImageLayout newLayout);

        void copyFrom(const Buffer &buffer);

        [[nodiscard]] std::shared_ptr<LogicalDevice> getDevice() const;

        [[nodiscard]] VkImage getImage() const;

        [[nodiscard]] VmaAllocation getAllocation() const;

        [[nodiscard]] VkFormat getFormat() const;

        [[nodiscard]] VkImageUsageFlags getUsageFlags() const;
    };
}
