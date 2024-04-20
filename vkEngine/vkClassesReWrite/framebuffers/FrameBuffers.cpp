#include "FrameBuffers.h"

FrameBuffers::FrameBuffers(
    VkDevice device,
    std::vector<VkImageView> swapChainImageViews,
    VkExtent2D swapChainExtent,
    VkRenderPass renderPass,
    std::shared_ptr<VkConfig> config
)
{
    m_Device = device;
    m_SwapChainImageViews = swapChainImageViews;
    m_SwapChainExtent = swapChainExtent;
    m_RenderPass = renderPass;
    m_Config = config;

    attachmentViewSets.resize(m_SwapChainImageViews.size());
}

void FrameBuffers::create(VkImageView colorImageView, VkImageView depthImageView)
{
    frameBuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {

        std::vector<VkImageView> attachments = {
                 m_SwapChainImageViews[i]
        };

        if (colorImageView != NULL && depthImageView != NULL) {
            attachments = {
                colorImageView,
                depthImageView,
                m_SwapChainImageViews[i]
            };
        }
        else if (colorImageView != NULL) {
            attachments = {
                colorImageView,
                m_SwapChainImageViews[i]
            };
        }
        else if (depthImageView != NULL) {
            attachments = {
                m_SwapChainImageViews[i],
                depthImageView
            };
        }

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_SwapChainExtent.width;
        framebufferInfo.height = m_SwapChainExtent.height;
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


