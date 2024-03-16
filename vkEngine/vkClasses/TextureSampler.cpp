#include "TextureSampler.h"

TextureSampler::TextureSampler(std::shared_ptr<TextureImage> p_TextureImage)
{
    this->p_TextureImage = p_TextureImage;
    this->p_FrameBuffers = p_TextureImage->p_FrameBuffers;
    this->p_CommandPool = p_FrameBuffers->p_CommandPool;
    this->p_GraphicsPipeline = p_CommandPool->p_GraphicsPipeline;
    this->p_DescriptorSetLayout = p_GraphicsPipeline->p_DescriptorSetLayout;
    this->p_RenderPass = p_DescriptorSetLayout->p_RenderPass;
    this->p_ImageViews = p_RenderPass->p_ImageViews;
    this->p_SwapChain = p_ImageViews->p_SwapChain;
    this->p_LogicalDevice = p_SwapChain->p_LogicalDevice;
    this->p_PhysicalDevice = p_LogicalDevice->p_PhysicalDevice;
    this->p_Surface = p_PhysicalDevice->p_Surface;
    this->p_Instance = p_Surface->p_Instance;
}

void TextureSampler::create()
{
    VkPhysicalDeviceProperties  properties{};
    vkGetPhysicalDeviceProperties(p_PhysicalDevice->physicalDevice, &properties);

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
    samplerInfo.maxLod = static_cast<float>(p_TextureImage->mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(p_LogicalDevice->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}
