#include "TextureSampler.h"

TextureSampler::TextureSampler(std::shared_ptr<TextureImage> textureImageObj)
{
    this->textureImageObj = textureImageObj;

    std::shared_ptr<LogicalDevice> logicalDeviceObj = textureImageObj->
        frameBuffersObj->
        depthResourcesObj->
        colorResourcesObj->
        commandPoolObj->
        graphicsPipelineObj->
        descriptorSetLayoutObj->
        renderPassObj->
        imageViewsObj->
        swapChainObj->
        logicalDeviceObj;


    VkPhysicalDeviceProperties  properties{};
    vkGetPhysicalDeviceProperties(logicalDeviceObj->physicalDeviceObj->physicalDevice, &properties);

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
    samplerInfo.maxLod = static_cast<float>(textureImageObj->mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(logicalDeviceObj->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}
