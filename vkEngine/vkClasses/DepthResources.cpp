#include "DepthResources.h"

DepthResources::DepthResources(std::shared_ptr<ColorResources> colorResourcesObj)
{
    this->colorResourcesObj = colorResourcesObj;

    create();
}

/*
Validation Error: [ VUID-VkFramebufferCreateInfo-pAttachments-00881 ]
Object 0: handle = 0xcfef35000000000a,
type = VK_OBJECT_TYPE_RENDER_PASS; 
Object 1: handle = 0xd897d90000000016, 
type = VK_OBJECT_TYPE_IMAGE_VIEW; 
Object 2: handle = 0x9fde6b0000000014, 
type = VK_OBJECT_TYPE_IMAGE; 
| MessageID = 0x2ff52eec | 
vkCreateFramebuffer(): pCreateInfo->pAttachments[1] has VK_SAMPLE_COUNT_16_BIT 
samples that do not match the VK_SAMPLE_COUNT_1_BIT samples used by 
the corresponding attachment for VkRenderPass 0xcfef35000000000a[]. 

The Vulkan spec states: If flags does not include VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT, 
each element of pAttachments must have been created with a samples value that matches 
the samples value specified by the corresponding VkAttachmentDescription in renderPass 
(https://vulkan.lunarg.com/doc/view/1.3.275.0/windows/1.3-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-pAttachments-00881)
*/
void DepthResources::create()
{
    std::shared_ptr<RenderPass> renderPassObj = colorResourcesObj->commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj;

    VkFormat depthFormat = renderPassObj->findDepthFormat();

    std::shared_ptr<SwapChain> swapChainObj = renderPassObj->imageViewsObj->swapChainObj;

    swapChainObj->logicalDeviceObj->createImage(
        swapChainObj->swapChainExtent.width,
        swapChainObj->swapChainExtent.height,
        depthFormat,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthImage,
        depthImageMemory,
        1,
        swapChainObj->logicalDeviceObj->physicalDeviceObj->msaaSamples
    );

    depthImageView = renderPassObj->imageViewsObj->createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}
