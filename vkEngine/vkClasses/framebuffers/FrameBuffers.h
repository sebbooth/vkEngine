#pragma once

#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <stdexcept>

#include "initialization/VkConfig.h"
#include "presentation/SwapChain.h"
#include "images/ImageViews.h"

class FrameBuffers
{
public:
	std::vector<VkFramebuffer> frameBuffers;

private:
	VkDevice m_Device{};
	std::shared_ptr<SwapChain> mp_SwapChain;
	std::shared_ptr<ImageViews> mp_ImageViews;
	VkRenderPass m_RenderPass{};
	std::shared_ptr<VkConfig> m_Config;

	std::vector<VkImageView> m_Attachments;


public:
	FrameBuffers(
		VkDevice device,
		std::shared_ptr<SwapChain> p_SwapChain,
		std::shared_ptr<ImageViews> p_ImageViews,
		VkRenderPass renderPass,
		std::shared_ptr<VkConfig> config
	);
	void create(
		VkImageView colorImageView = NULL, 
		VkImageView depthImageView = NULL
	);
	void destroy();
};

#endif