#include "FrameBuffers.h"

FrameBuffers::FrameBuffers(std::shared_ptr<DepthResources> depthResourcesObj)
{
    this->depthResourcesObj = depthResourcesObj;
    create();
}

void FrameBuffers::create()
{
    std::shared_ptr<ImageViews> imageViewsObj = depthResourcesObj->colorResourcesObj->commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->imageViewsObj;

    imageViewsObj->swapChainObj->swapChainFramebuffers.resize(imageViewsObj->swapChainImageViews.size());

    for (size_t i = 0; i < imageViewsObj->swapChainImageViews.size(); i++) {
        std::array<VkImageView, 3> attachments = {
            depthResourcesObj->colorResourcesObj->colorImageView,
            depthResourcesObj->depthImageView,
            imageViewsObj->swapChainImageViews[i],
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = depthResourcesObj->colorResourcesObj->commandPoolObj->graphicsPipelineObj->descriptorSetLayoutObj->renderPassObj->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = imageViewsObj->swapChainObj->swapChainExtent.width;
        framebufferInfo.height = imageViewsObj->swapChainObj->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(imageViewsObj->swapChainObj->logicalDeviceObj->device, &framebufferInfo, nullptr, &imageViewsObj->swapChainObj->swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}
