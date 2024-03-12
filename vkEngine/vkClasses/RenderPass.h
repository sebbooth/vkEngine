#pragma once
#include "ImageViews.h"

class RenderPass
{
public:
	std::shared_ptr<ImageViews> imageViewsObj;

	VkRenderPass renderPass{};

	RenderPass(std::shared_ptr<ImageViews> imageViewsObj);

	VkFormat findDepthFormat();

private:
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
};

