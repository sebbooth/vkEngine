#pragma once

#ifndef COMMANDBUFFERS_H
#define COMMANDBUFFERS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <array>
#include <memory>
#include <stdexcept>

#include "initialization/VkConfig.h"

class CommandBuffers
{
	public:
		std::vector<VkCommandBuffer> commandBuffers;

	private:
		VkDevice m_Device;
		VkCommandPool m_CommandPool;
		std::shared_ptr<VkConfig> m_Config;

		VkRenderPass m_RenderPass;
		VkPipeline m_GraphicsPipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkFramebuffer> m_FrameBuffers;

		std::vector<VkDescriptorSet> m_DescriptorSets;
		VkBuffer m_VertexBuffer;
		VkBuffer m_IndexBuffer;

		VkExtent2D m_Extent;

	public:
		CommandBuffers(
			VkDevice device,
			VkCommandPool commandPool,
			std::shared_ptr<VkConfig> config
		);
		void create();
	
		void attachAllResources(
			VkRenderPass renderPass,
			VkPipeline graphicsPipeline,
			VkPipelineLayout pipelineLayout,
			std::vector<VkFramebuffer> frameBuffers,
			std::vector<VkDescriptorSet> descriptorSets,
			VkBuffer vertexBuffer = {},
			VkBuffer indexBuffer = {}
		);
		void attachRenderPass(VkRenderPass renderPass);
		void attachGraphicsPipeline(VkPipeline graphicsPipeline, VkPipelineLayout pipelineLayout);
		void attachFrameBuffers(std::vector<VkFramebuffer> frameBuffers);
		void attachDescriptorSets(std::vector<VkDescriptorSet> descriptorSets);
		void attachVertexBuffer(VkBuffer vertexBuffer);
		void attachIndexBuffer(VkBuffer indexBuffer);
		void setExtent(VkExtent2D extent);
		void attachGui();

		void recordBufferVertices(
			VkCommandBuffer commandBuffer,
			std::vector<VkBuffer> shaderStorageBuffers,
			uint32_t currentFrame,
			uint32_t imageIndex,
			int vertexCount
		);

		void recordBufferIndexed(
			uint32_t currentFrame, 
			uint32_t imageIndex, 
			uint32_t indexCount
		);

		void recordComputeBuffer(
			VkCommandBuffer commandBuffer,
			std::vector<VkBuffer> shaderStorageBuffers,
			uint32_t currentFrame,
			uint32_t imageIndex,
			int PARTICLE_COUNT
		);
		void resetBuffer(uint32_t currentFrame);
};

#endif