#pragma once

#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include "FrameBuffers.h"
#include "UniformBuffers.h"
#include "TextureSampler.h"

class DescriptorPool
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;
	std::shared_ptr<RenderPass> p_RenderPass;
	std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout;
	std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
	std::shared_ptr<CommandPool> p_CommandPool;
	std::shared_ptr<FrameBuffers> p_FrameBuffers;

	std::shared_ptr<UniformBuffers> p_UniformBuffers;
	std::shared_ptr<TextureImage> p_TextureImage;
	std::shared_ptr<TextureSampler> p_TextureSampler;

	VkDescriptorPool descriptorPool{};
	std::vector<VkDescriptorPoolSize> poolSizes{};

	DescriptorPool(std::shared_ptr<FrameBuffers> p_FrameBuffers);
	void create();
	void update(uint32_t currentImage);
	void destroy();
};

#endif