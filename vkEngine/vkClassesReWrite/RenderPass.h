#pragma once

#ifndef RENDERPASS_H
#define RENDERPASS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <memory>

#include "RenderingSettings.h"

class RenderPass
{
public:
	VkRenderPass renderPass{};
	uint32_t subpass;

private:
	VkPhysicalDevice m_PhysicalDevice;
	VkSampleCountFlagBits m_MsaaSamples;

	VkDevice m_Device;

	VkFormat m_SwapChainImageFormat;

	std::shared_ptr<RenderingSettings> m_RS;

public:
	RenderPass(
		VkPhysicalDevice physicalDevice,
		VkSampleCountFlagBits msaaSamples,
		VkDevice device,
		VkFormat swapChainImageFormat,
		std::shared_ptr<RenderingSettings> RS
	);
	void create();
	void destroy();
	VkFormat findDepthFormat();

private:
	VkFormat findSupportedFormat(
		const std::vector<VkFormat>& candidates, 
		VkImageTiling tiling, 
		VkFormatFeatureFlags features
	);
};

#endif