#pragma once

#ifndef FRAMEBUFFERS_H
#define FRAMEBUFFERS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>
#include <stdexcept>

#include "initialization/VkConfig.h"

class FrameBuffers
{
public:
	std::vector<VkFramebuffer> frameBuffers;

private:
	std::vector<std::vector<VkImageView>> attachmentViewSets;

	VkDevice m_Device{};
	std::vector<VkImageView> m_SwapChainImageViews;
	VkExtent2D m_SwapChainExtent{};
	VkRenderPass m_RenderPass{};
	std::shared_ptr<VkConfig> m_Config;

	std::vector<VkImageView> m_Attachments;


public:
	FrameBuffers(
		VkDevice device,
		std::vector<VkImageView> swapChainImageViews,
		VkExtent2D swapChainExtent,
		VkRenderPass renderPass,
		std::shared_ptr<VkConfig> config
	);
	void create();
	void destroy();

	void bindSwapChainImageViews(uint32_t binding);
	void bindColorReources(uint32_t binding, VkImageView colorImageView);
	void bindDepthResources(uint32_t binding, VkImageView depthImageView);
};

#endif