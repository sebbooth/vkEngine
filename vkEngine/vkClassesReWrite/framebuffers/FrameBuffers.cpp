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

void FrameBuffers::create()
{
    frameBuffers.resize(m_SwapChainImageViews.size());

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachmentViewSets[i].size());
        framebufferInfo.pAttachments = attachmentViewSets[i].data();
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

void FrameBuffers::bindSwapChainImageViews(uint32_t binding)
{
    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
        if (attachmentViewSets[i].size() <= binding) {
            attachmentViewSets[i].resize(binding + 1);
            attachmentViewSets[i][binding] = m_SwapChainImageViews[i];
        }
    }
}

void FrameBuffers::bindColorReources(uint32_t binding, VkImageView colorImageView)
{
    if (!m_Config->msaaEnabled) {
        throw std::runtime_error("Attempting to attach color resolve image view with MSAA disabled!");
    }

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
        if (attachmentViewSets[i].size() <= binding) {
            attachmentViewSets[i].resize(binding + 1);
            attachmentViewSets[i][binding] = colorImageView;
        }
    }
}

void FrameBuffers::bindDepthResources(uint32_t binding, VkImageView depthImageView)
{
    if (!m_Config->depthStencilEnabled) {
        throw std::runtime_error("Attempting to attach depth image view with depth stencil disabled!");
    }

    for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
        if (attachmentViewSets[i].size() <= binding) {
            attachmentViewSets[i].resize(binding + 1);
            attachmentViewSets[i][binding] = depthImageView;
        }
    }
}


