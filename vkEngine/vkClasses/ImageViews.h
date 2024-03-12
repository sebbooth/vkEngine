#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "SwapChain.h"

class ImageViews
{
public:
	std::shared_ptr<SwapChain> swapChainObj;

	std::vector<VkImageView> swapChainImageViews;

	ImageViews(std::shared_ptr<SwapChain> swapChainObj);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void create();
	void destroyImageViews();
};

