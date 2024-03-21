#pragma once

#ifndef RENDERPASS_H
#define RENDERPASS_H

#include "ImageViews.h"
#include <vector>

class RenderPass
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;

	VkRenderPass renderPass{};

	bool depthEnabled = true;
	//uint32_t attachmentCount = -1;
	uint32_t subpass;

	RenderPass(std::shared_ptr<ImageViews> p_ImageViews);
	void create();
	void destroy();
	VkFormat findDepthFormat();

private:
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};

#endif