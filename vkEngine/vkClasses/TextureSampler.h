#pragma once

#ifndef TEXTURESAMPLER_H
#define TEXTURESAMPLER_H

#include "TextureImage.h"

class TextureSampler
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
	std::shared_ptr<TextureImage> p_TextureImage;

	VkSampler textureSampler{};

	TextureSampler(std::shared_ptr<TextureImage> p_TextureImage);
	void create();
};

#endif