#include "FrameBuffers.h"

FrameBuffers::FrameBuffers(
    VkDevice device,
    std::shared_ptr<SwapChain> p_SwapChain,
    std::shared_ptr<ImageViews> p_ImageViews,
    VkRenderPass renderPass, 
    std::shared_ptr<VkConfig> config
)
{
    m_Device = device;
    mp_SwapChain = p_SwapChain;
    mp_ImageViews = p_ImageViews;
    m_RenderPass = renderPass;
    m_Config = config;
}

void FrameBuffers::create(
    VkImageView colorImageView, 
    VkImageView depthImageView
)
{
    frameBuffers.resize(mp_ImageViews->swapChainImageViews.size());

    for (size_t i = 0; i < mp_ImageViews->swapChainImageViews.size(); i++) {

        std::vector<VkImageView> attachments = {
                 mp_ImageViews->swapChainImageViews[i]
        };

        if (colorImageView != NULL && depthImageView != NULL) {
            attachments = {
                colorImageView,
                depthImageView,
                mp_ImageViews->swapChainImageViews[i]
            };
        }
        else if (colorImageView != NULL) {
            attachments = {
                colorImageView,
                mp_ImageViews->swapChainImageViews[i]
            };
        }
        else if (depthImageView != NULL) {
            attachments = {
                mp_ImageViews->swapChainImageViews[i],
                depthImageView
            };
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = mp_SwapChain->swapChainExtent.width;
        framebufferInfo.height = mp_SwapChain->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &frameBuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void FrameBuffers::destroy()
{
    for (auto framebuffer : frameBuffers) {
        vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
    }
}


