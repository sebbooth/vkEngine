#pragma once

#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H

#include "DescriptorSets.h"
#include "IndexBuffer.h"
#include "Gui.h"

class CommandBuffers
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

	std::shared_ptr<VertexBuffer> p_VertexBuffer;
	std::shared_ptr<IndexBuffer> p_IndexBuffer;
	std::shared_ptr<Gui> p_Gui;

	bool guiEnabled = false;

	std::vector<VkCommandBuffer> commandBuffers;

	CommandBuffers(std::shared_ptr<DescriptorSets> p_DescriptorSets);
	void create();

	void attachVertexBuffer(std::shared_ptr<VertexBuffer> p_VertexBuffer);
	void attachIndexBuffer(std::shared_ptr<IndexBuffer> p_IndexBuffer);
	void attachGui(std::shared_ptr<Gui> p_Gui);

	void recordBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, uint32_t imageIndex, std::vector<VkBuffer> shaderStorageBuffers, int PARTICLE_COUNT);

	void recordBuffer(uint32_t currentFrame, uint32_t imageIndex);
	void recordComputeBuffer(
		uint32_t currentFrame, 
		VkCommandBuffer commandBuffer, 
		uint32_t imageIndex, 
		std::vector<VkBuffer> shaderStorageBuffers, 
		int PARTICLE_COUNT
	);
	void resetBuffer(uint32_t currentFrame);
};

#endif