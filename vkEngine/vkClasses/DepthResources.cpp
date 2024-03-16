#include "DepthResources.h"

DepthResources::DepthResources(std::shared_ptr<CommandPool> p_CommandPool)
{
    this->p_CommandPool = p_CommandPool;
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

void DepthResources::create()
{
    if (p_RenderPass->depthEnabled) {
        VkFormat depthFormat = p_RenderPass->findDepthFormat();

        p_SwapChain->p_LogicalDevice->createImage(
            p_SwapChain->swapChainExtent.width,
            p_SwapChain->swapChainExtent.height,
            depthFormat,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            depthImage,
            depthImageMemory,
            1,
            p_PhysicalDevice->msaaSamples
        );

        depthImageView = p_ImageViews->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
    }
}
