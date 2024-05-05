#pragma once

#ifndef IMAGEVIEWS_H
#define IMAGEVIEWS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>

class ImageViews
{
public:
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	std::vector<VkImageView> swapChainImageViews;

private:
	VkDevice m_Device;

public:
	ImageViews(VkDevice device);

	VkImageView createImageView(
		VkImage image, 
		VkFormat format, 
		VkImageAspectFlags aspectFlags, 
		uint32_t mipLevels
	);

	void createSwapChainImageViews(
		std::vector<VkImage> swapChainImages, 
		VkFormat swapChainImageFormat
	);

	void destroySwapChainImageViews();

};

#endif