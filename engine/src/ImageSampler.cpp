#include "ImageSampler.h"

namespace Vixen {
    ImageSampler::ImageSampler(const std::shared_ptr<LogicalDevice>& device) : device(device), sampler() {
        VkSamplerCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter = VK_FILTER_LINEAR;
        info.minFilter = VK_FILTER_LINEAR;
        info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.anisotropyEnable = VK_FALSE;
        info.maxAnisotropy = 1;
        info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        info.unnormalizedCoordinates = VK_FALSE;
        info.compareEnable = VK_FALSE;
        info.compareOp = VK_COMPARE_OP_ALWAYS;
        info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.mipLodBias = 0.0f;
        info.minLod = 0.0f;
        info.maxLod = 0.0f;

        VK_CHECK_RESULT(vkCreateSampler(device->device, &info, nullptr, &sampler))
    }

    ImageSampler::~ImageSampler() {
        vkDestroySampler(device->device, sampler, nullptr);
    }

    VkSampler ImageSampler::getSampler() const {
        return sampler;
    }
}
