#include "DepthResources.h"

DepthResources::DepthResources(
    VkDevice device,
    std::shared_ptr<Images> p_Images,
    std::shared_ptr<ImageViews> p_ImageViews,
    VkExtent2D swapChainExtent,
    VkFormat depthFormat,
    VkSampleCountFlagBits msaaSamples
)
{
    m_Device = device;
    mp_Images = p_Images;
    mp_ImageViews = p_ImageViews;
    m_SwapChainExtent = swapChainExtent;
    m_DepthFormat = depthFormat;
    m_MsaaSamples = msaaSamples;
}

void DepthResources::create()
{
    mp_Images->createImage(
        m_SwapChainExtent.width,
        m_SwapChainExtent.height,
        m_DepthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory,
        1,
        m_MsaaSamples
    );

    depthImageView = mp_ImageViews->createImageView(
        depthImage, 
        m_DepthFormat, 
        VK_IMAGE_ASPECT_DEPTH_BIT, 
        1
    );
}

void DepthResources::destroy()
{
    vkDestroyImageView(m_Device, depthImageView, nullptr);
    vkDestroyImage(m_Device, depthImage, nullptr);
    vkFreeMemory(m_Device, depthImageMemory, nullptr);
}
  