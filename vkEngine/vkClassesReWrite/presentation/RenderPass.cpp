#include "RenderPass.h"

RenderPass::RenderPass(
    VkPhysicalDevice physicalDevice,
    VkSampleCountFlagBits msaaSamples,
    VkDevice device,
    VkFormat swapChainImageFormat,
    std::shared_ptr<VkConfig> config
)
{
    m_PhysicalDevice = physicalDevice;
    m_MsaaSamples = msaaSamples;
    m_Device = device;
    m_SwapChainImageFormat = swapChainImageFormat;
    m_Config = config;
}

void RenderPass::create()
{
    if (m_ColorAttachmentIndex != -1) {
        if (m_Attachments.size() <= m_ColorAttachmentIndex) {
            m_Attachments.resize(m_ColorAttachmentIndex + 1);
            m_Attachments[m_ColorAttachmentIndex] = m_ColorAttachment;
        }
    }

    if (m_DepthAttachmentIndex != -1) {
        if (m_Attachments.size() <= m_DepthAttachmentIndex) {
            m_Attachments.resize(m_DepthAttachmentIndex + 1);
            m_Attachments[m_DepthAttachmentIndex] = m_DepthAttachment;
        }
    }
    
    if (m_ColorAttachmentResolveIndex != -1) {
        if (m_Attachments.size() <= m_ColorAttachmentResolveIndex) {
            m_Attachments.resize(m_ColorAttachmentResolveIndex + 1);
            m_Attachments[m_ColorAttachmentResolveIndex] = m_ColorAttachmentResolve;
        }
    }

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = m_DstAccessMask;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(m_Attachments.size());
    renderPassInfo.pAttachments = m_Attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &m_Subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void RenderPass::destroy()
{
    vkDestroyRenderPass(m_Device, renderPass, nullptr);
}

uint32_t RenderPass::bindColorPass()
{
    m_Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    m_ColorAttachment.format = m_SwapChainImageFormat;
    m_ColorAttachment.samples = m_MsaaSamples;
    m_ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    m_ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    m_ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    m_ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    m_ColorAttachmentRef.attachment = m_AttachmentCount++;
    m_ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_Subpass.colorAttachmentCount = 1;
    m_Subpass.pColorAttachments = &m_ColorAttachmentRef;

    m_DstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    m_ColorAttachmentIndex = m_AttachmentCount - 1;
    return m_ColorAttachmentIndex;
}

uint32_t RenderPass::bindDepthPass()
{
    if (!m_Config->depthStencilEnabled) {
        throw std::runtime_error("Attempting to attach depth pass with depth stencil disabled!");
    }
    m_DepthAttachment.format = findDepthFormat();
    m_DepthAttachment.samples = m_MsaaSamples;
    m_DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    m_DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    m_DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    m_DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    m_DepthAttachmentRef.attachment = m_AttachmentCount++;
    m_DepthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    m_Subpass.pDepthStencilAttachment = &m_DepthAttachmentRef;

    m_DstAccessMask = m_DstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    m_DepthAttachmentIndex = m_AttachmentCount - 1;
    return m_DepthAttachmentIndex;
}

uint32_t RenderPass::bindMsaaPass()
{
    if (!m_Config->msaaEnabled) {
        throw std::runtime_error("Attempting to attach color resolve pass with MSAA disabled!");
    }
    m_ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_ColorAttachmentResolve.format = m_SwapChainImageFormat;
    m_ColorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    m_ColorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_ColorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    m_ColorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_ColorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    m_ColorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_ColorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    m_ColorAttachmentResolveRef.attachment = m_AttachmentCount++;
    m_ColorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    m_Subpass.pResolveAttachments = &m_ColorAttachmentResolveRef;

    m_ColorAttachmentResolveIndex = m_AttachmentCount - 1;
    return m_ColorAttachmentResolveIndex;
}

VkFormat RenderPass::findDepthFormat()
{
    return findSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkFormat RenderPass::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
}
