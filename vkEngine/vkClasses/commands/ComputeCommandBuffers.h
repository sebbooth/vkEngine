#pragma once

#ifndef COMPUTECOMMANDBUFFERS_H
#define COMPUTECOMMANDBUFFERS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <cmath>
#include <stdexcept>
#include <iostream>

#include "initialization/VkConfig.h"

class ComputeCommandBuffers
{
	public:
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkCommandBuffer> commandBuffers1;
		std::vector<VkCommandBuffer> commandBuffers2;

	private:
		VkDevice m_Device;
		VkCommandPool m_CommandPool;
		std::shared_ptr<VkConfig> m_Config;

		VkPipeline m_ComputePipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		uint32_t groupSizeX = 256;
		uint32_t groupSizeY = 1;
		uint32_t groupSizeZ = 1;

public:
	ComputeCommandBuffers(
		VkDevice device,
		VkCommandPool commandPool,
		std::shared_ptr<VkConfig> config
	);

	void create();
	void attachComputePipeline(VkPipeline computePipeline, VkPipelineLayout pipelineLayout);
	void attachDescriptorSets(std::vector<VkDescriptorSet> descriptorSets);
	void setExtent(unsigned int width, unsigned int height);
	void setGroupSize(unsigned int size);

	void recordBuffer(
		VkCommandBuffer commandBuffer, 
		uint32_t currentFrame, 
		VkEvent syncEvent, 
		bool barrier, 
		uint32_t queue, 
		VkImage depthImage, 
		VkImage canvasImage
	);

	//void recordBuffer(VkCommandBuffer commandBuffer, uint32_t currentFrame, VkEvent syncEvent, bool barrier = false);
};

#endif