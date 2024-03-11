#include "ImageViews.h"

ImageViews::ImageViews(std::shared_ptr<SwapChain> swapChainObj)
{
    this->swapChainObj = swapChainObj;
    create();
}

VkImageView ImageViews::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(swapChainObj->logicalDeviceObj->device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    return imageView;
}

void ImageViews::create()
{
    swapChainImageViews.resize(swapChainObj->swapChainImages.size());

    for (uint32_t i = 0; i < swapChainObj->swapChainImages.size(); i++) {
        swapChainImageViews[i] = createImageView(swapChainObj->swapChainImages[i], swapChainObj->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void ImageViews::destroyImageViews()
{
    for (auto imageView : swapChainImageViews) {
        vkDestroyImageView(swapChainObj->logicalDeviceObj->device, imageView, nullptr);
    }
}
