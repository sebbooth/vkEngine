#pragma once

#ifndef DESCRIPTORSETS_H
#define DESCRIPTORSETS_H

#include "DescriptorPool.h"

class DescriptorSets
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;
	std::shared_ptr<RenderPass> p_RenderPass;
	std::shared_ptr<DescriptorSetLayout> p_GraphicsDescriptorSetLayout;
	std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
	std::shared_ptr<CommandPool> p_CommandPool;
	std::shared_ptr<FrameBuffers> p_FrameBuffers;
	std::shared_ptr<DescriptorPool> p_DescriptorPool;

	std::vector<VkDescriptorSet> descriptorSets;

	DescriptorSets(std::shared_ptr<DescriptorPool> p_DescriptorPool);
	void create();
	void createWithStorageSampler(std::vector<VkImageView> imageViews, std::vector<VkSampler> imageSamplers);
	void destroy();
};

#endif