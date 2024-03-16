#include "ImageViews.h"

ImageViews::ImageViews(std::shared_ptr<SwapChain> p_SwapChain)
{
    this->p_SwapChain = p_SwapChain;
    this->p_LogicalDevice = p_SwapChain->p_LogicalDevice;
    this->p_PhysicalDevice = p_LogicalDevice->p_PhysicalDevice;
    this->p_Surface = p_PhysicalDevice->p_Surface;
    this->p_Instance = p_Surface->p_Instance;
}

VkImageView ImageViews::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(p_LogicalDevice->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
}

void ImageViews::create()
{
    swapChainImageViews.resize(p_SwapChain->swapChainImages.size());

    for (uint32_t i = 0; i < p_SwapChain->swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(p_SwapChain->swapChainImages[i], p_SwapChain->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

void ImageViews::destroyImageViews()
{
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(p_LogicalDevice->device, imageView, nullptr);
    }
}
