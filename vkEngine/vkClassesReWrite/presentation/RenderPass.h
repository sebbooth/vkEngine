#pragma once

#ifndef RENDERPASS_H
#define RENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <memory>

#include "initialization/VkConfig.h"

class RenderPass
{
public:
	VkRenderPass renderPass{};
	uint32_t subpass;

private:
	VkSubpassDescription m_Subpass{};
	std::vector<VkAttachmentDescription> m_Attachments;
	uint32_t m_AttachmentCount = 0;
	VkAccessFlags m_DstAccessMask;


	VkAttachmentDescription m_ColorAttachment{};
	VkAttachmentReference m_ColorAttachmentRef{};
	uint32_t m_ColorAttachmentIndex = -1;
	VkAttachmentDescription m_DepthAttachment{};
	VkAttachmentReference m_DepthAttachmentRef{};
	uint32_t m_DepthAttachmentIndex = -1;
	VkAttachmentDescription m_ColorAttachmentResolve{};
	VkAttachmentReference m_ColorAttachmentResolveRef{};
	uint32_t m_ColorAttachmentResolveIndex = -1;

	VkPhysicalDevice m_PhysicalDevice;
	VkSampleCountFlagBits m_MsaaSamples;
	VkDevice m_Device;
	VkFormat m_SwapChainImageFormat;
	std::shared_ptr<VkConfig> m_Config;

public:
	RenderPass(
		VkPhysicalDevice physicalDevice,
		VkSampleCountFlagBits msaaSamples,
		VkDevice device,
		VkFormat swapChainImageFormat,
		std::shared_ptr<VkConfig> config
	);
	void create();
	void destroy();

	uint32_t bindColorPass();
	uint32_t bindDepthPass();
	uint32_t bindMsaaPass();

	VkFormat findDepthFormat();

private:
	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, 
		VkFormatFeatureFlags features
	);
};

#endif