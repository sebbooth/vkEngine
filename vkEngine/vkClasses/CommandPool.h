#pragma once

#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include "GraphicsPipeline.h"
#include "ComputeGraphicsPipeline.h"

class CommandPool
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

	VkCommandPool commandPool{};

	CommandPool(std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline);

	void create();
	void destroy();

	VkCommandBuffer beginSingleTimeCommands();

	void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};

#endif