#pragma once
#include "SwapChain.h"

class ImageViews
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	std::vector<VkImageView> swapChainImageViews;

	ImageViews(std::shared_ptr<SwapChain> p_SwapChain);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void create();
	void destroyImageViews();
};

