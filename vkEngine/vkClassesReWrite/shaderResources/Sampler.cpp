#include "Sampler.h"

Sampler::Sampler(VkPhysicalDevice physicalDevice, VkDevice device, std::shared_ptr<VkConfig> config)
{
	m_PhysicalDevice = physicalDevice;
	m_Device = device;
	m_Config = config;
}

void Sampler::create()
{
    VkPhysicalDeviceProperties  properties{};
    vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

    imageSamplers.resize(m_Config->MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(1);
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &imageSamplers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }
}

void Sampler::destroy()
{
    for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySampler(m_Device, imageSamplers[i], nullptr);
    }
}
