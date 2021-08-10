#include "Vulkan.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "LogicalDevice.h"

namespace Vixen::Vk {
    class Image {
        VmaAllocation allocation;

        uint32_t width;

        uint32_t height;

        VkImageLayout layout;

        VkImageUsageFlags usageFlags;

    protected:
        const std::shared_ptr<LogicalDevice> device;

        VkImage image;

        VkFormat format;

    public:
        Image(const std::shared_ptr<LogicalDevice> &device, uint32_t width, uint32_t height, VkFormat format,
              VkImageTiling tiling, VkImageUsageFlags usageFlags);

        Image(const Image &other) = delete;

        Image(Image &&other) noexcept;

        ~Image();

        static Image from(const std::shared_ptr<LogicalDevice> &device, const std::string &path);

        void transition(VkImageLayout newLayout);

        void copyFrom(const Buffer &buffer);

        [[nodiscard]] std::shared_ptr<LogicalDevice> getDevice() const;

        [[nodiscard]] VkFormat getFormat() const;

        [[nodiscard]] VkImageUsageFlags getUsageFlags() const;
    };
}
