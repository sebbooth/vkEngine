#pragma once

#ifndef DEPTHRESOURCES_H
#define DEPTHRESOURCES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "images/Images.h"
#include "images/ImageViews.h"


class DepthResources
{
public:
	VkImage depthImage{};
	VkDeviceMemory depthImageMemory{};
	VkImageView depthImageView{};

private:
	VkDevice m_Device{};

	std::shared_ptr<Images> mp_Images;
	std::shared_ptr<ImageViews> mp_ImageViews;

	VkExtent2D m_SwapChainExtent{};
	VkFormat m_DepthFormat{};
	VkSampleCountFlagBits m_MsaaSamples{};

public:
	DepthResources(
		VkDevice device,
		std::shared_ptr<Images> p_Images,
		std::shared_ptr<ImageViews> p_ImageViews,
		VkExtent2D swapChainExtent,
		VkFormat depthFormat,
		VkSampleCountFlagBits msaaSamples
	);
	void create();
	void destroy();
};

#endif