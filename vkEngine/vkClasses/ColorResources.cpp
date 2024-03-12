#include "ColorResources.h"

ColorResources::ColorResources(std::shared_ptr<CommandPool> commandPoolObj)
{
    this->commandPoolObj = commandPoolObj;
    create();
}

void ColorResources::create()
{
    std::shared_ptr<SwapChain> swapChainObj = commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->imageViewsObj->swapChainObj;

    VkFormat colorFormat = swapChainObj->swapChainImageFormat;


    
    swapChainObj->logicalDeviceObj->createImage(
        swapChainObj->swapChainExtent.width, 
        swapChainObj->swapChainExtent.height,
        colorFormat, 
        VK_IMAGE_TILING_OPTIMAL, 
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
        colorImage, 
        colorImageMemory,
        1,
        swapChainObj->logicalDeviceObj->physicalDeviceObj->msaaSamples
    );

    colorImageView = commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->imageViewsObj->createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}
