#pragma once
#include "SwapChain.h"

class ImageViews
{
public:
	std::shared_ptr<SwapChain> swapChainObj;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	std::vector<VkImageView> swapChainImageViews;

	ImageViews(std::shared_ptr<SwapChain> swapChainObj);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void create();
	void destroyImageViews();
};

