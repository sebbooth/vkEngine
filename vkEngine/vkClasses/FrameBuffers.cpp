#include "FrameBuffers.h"

FrameBuffers::FrameBuffers(std::shared_ptr<CommandPool> p_CommandPool)
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

void FrameBuffers::create()
{
    if (p_PhysicalDevice->msaaEnabled) {
        p_ColorResources = std::make_shared<ColorResources>(p_CommandPool);
        p_ColorResources->create();
    }
    if (p_RenderPass->depthEnabled) {
        p_DepthResources = std::make_shared<DepthResources>(p_CommandPool);
        p_DepthResources->create();
    }

    p_SwapChain->swapChainFramebuffers.resize(p_ImageViews->swapChainImageViews.size());

    for (size_t i = 0; i < p_ImageViews->swapChainImageViews.size(); i++) {

        // Really don't like this but using push.back kept causing issues - will need to figure out fix if adding more attachments
        std::vector<VkImageView> attachments = {
                p_ImageViews->swapChainImageViews[i]
        };

        if (p_PhysicalDevice->msaaEnabled && p_RenderPass->depthEnabled) {
            attachments = {
                p_ColorResources->colorImageView,
                p_DepthResources->depthImageView,
                p_ImageViews->swapChainImageViews[i]
            };
        }
        else if (p_PhysicalDevice->msaaEnabled) {
            attachments = {
                p_ColorResources->colorImageView,
                p_ImageViews->swapChainImageViews[i]
            };
        }
        else if (p_RenderPass->depthEnabled) {
            attachments = {
                p_ImageViews->swapChainImageViews[i],
                p_DepthResources->depthImageView
            };
        }
     
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = p_RenderPass->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = p_SwapChain->swapChainExtent.width;
        framebufferInfo.height = p_SwapChain->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(p_LogicalDevice->device, &framebufferInfo, nullptr, &p_SwapChain->swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FrameBuffers::destroy()
{
    if (p_PhysicalDevice->msaaEnabled) {
        vkDestroyImageView(p_LogicalDevice->device, p_ColorResources->colorImageView, nullptr);
        vkDestroyImage(p_LogicalDevice->device, p_ColorResources->colorImage, nullptr);
        vkFreeMemory(p_LogicalDevice->device, p_ColorResources->colorImageMemory, nullptr);
    }
    if (p_RenderPass->depthEnabled) {
        vkDestroyImageView(p_LogicalDevice->device, p_DepthResources->depthImageView, nullptr);
        vkDestroyImage(p_LogicalDevice->device, p_DepthResources->depthImage, nullptr);
        vkFreeMemory(p_LogicalDevice->device, p_DepthResources->depthImageMemory, nullptr);
    }
}


