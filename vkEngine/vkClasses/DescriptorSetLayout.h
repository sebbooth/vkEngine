#pragma once

#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#include "RenderPass.h"

class DescriptorSetLayout
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;
	std::shared_ptr<RenderPass> p_RenderPass;

	VkDescriptorSetLayout descriptorSetLayout{};

	bool uboEnabled = true;
	bool samplerEnabled = true;
	bool guiEnabled = true;

	DescriptorSetLayout(std::shared_ptr<RenderPass> p_RenderPass);
	void create();
	void destroy();

	DescriptorSetLayout() = default;
};

#endif