#pragma once

#ifndef COLORRESOURCES_H
#define COLORRESOURCES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "images/Images.h"
#include "images/ImageViews.h"

class ColorResources
{
	public:
		VkImage colorImage{};
		VkDeviceMemory colorImageMemory{};
		VkImageView colorImageView{};

	private:
		VkDevice m_Device{};

		std::shared_ptr<Images> mp_Images;
		std::shared_ptr<ImageViews> mp_ImageViews;

		VkExtent2D m_SwapChainExtent{};
		VkFormat m_ColorFormat{};
		VkSampleCountFlagBits m_MsaaSamples{};

	public:
		ColorResources(
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