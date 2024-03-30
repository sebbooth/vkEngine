#pragma once

#ifndef SYNCOBJECTS_H
#define SYNCOBJECTS_H

#include "CommandBuffers.h"

class SyncObjects
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
	std::shared_ptr<DescriptorPool> p_DescriptorPool;
	std::shared_ptr<DescriptorSets> p_DescriptorSets;
	std::shared_ptr<CommandBuffers> p_CommandBuffer;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkSemaphore> computeFinishedSemaphores;

	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> computeInFlightFences;

	SyncObjects(std::shared_ptr<CommandBuffers> p_CommandBuffer);
	void create();
	void destroy();
	void waitForFences(uint32_t currentFrame, VkBool32 waitAll, uint64_t timeout);
};

#endif