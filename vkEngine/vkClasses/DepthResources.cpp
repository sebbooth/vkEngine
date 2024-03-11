#include "DepthResources.h"

DepthResources::DepthResources(std::shared_ptr<CommandPool> commandPoolObj)
{
    this->renderPassObj = commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj;
    create();
}

void DepthResources::create()
{
    VkFormat depthFormat = renderPassObj->findDepthFormat();

    VkExtent2D swapChainExtent = renderPassObj->imageViewsObj->swapChainObj->swapChainExtent;

    renderPassObj->imageViewsObj->swapChainObj->logicalDeviceObj->createImage(
        swapChainExtent.width,
        swapChainExtent.height,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory
    );

    depthImageView = renderPassObj->imageViewsObj->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}
