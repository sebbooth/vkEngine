#include "ColorResources.h"

ColorResources::ColorResources(
    VkDevice device,
    std::shared_ptr<Images> p_Images,
    std::shared_ptr<ImageViews> p_ImageViews,
    VkExtent2D swapChainExtent,
    VkFormat colorFormat,
    VkSampleCountFlagBits msaaSamples
)
{
    m_Device = device;
    mp_Images = p_Images;
    mp_ImageViews = p_ImageViews;
    m_SwapChainExtent = swapChainExtent;
    m_ColorFormat = colorFormat;
    m_MsaaSamples = msaaSamples;
}

void ColorResources::create()
{
        mp_Images->createImage(
            m_SwapChainExtent.width,
            m_SwapChainExtent.height,
            m_ColorFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            colorImage,
            colorImageMemory,
            1,
            m_MsaaSamples
        );

        colorImageView = mp_ImageViews->createImageView(colorImage, m_ColorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void ColorResources::destroy()
{
    vkDestroyImageView(m_Device, colorImageView, nullptr);
    vkDestroyImage(m_Device, colorImage, nullptr);
    vkFreeMemory(m_Device, colorImageMemory, nullptr);
}
