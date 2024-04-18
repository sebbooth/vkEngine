#pragma once

#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "FileUtils.h"
#include "initialization/VkConfig.h"
#include "VkStructs.h"

class GraphicsPipeline
{
	public:
		VkPipeline graphicsPipeline{};
		VkPipelineLayout pipelineLayout{};

	private:
		VkSampleCountFlagBits m_MsaaSamples{};
		VkDevice m_Device{};
		VkExtent2D m_SwapChainExtent{};
		VkRenderPass m_RenderPass{};
		VkDescriptorSetLayout m_DescriptorSetLayout{};

		std::shared_ptr<VkConfig> m_Config;

	public:
		GraphicsPipeline(
			VkSampleCountFlagBits msaaSamples,
			VkDevice device,
			VkExtent2D swapChainExtent,
			VkRenderPass renderPass,
			VkDescriptorSetLayout descriptorSetLayout,
			std::shared_ptr<VkConfig> config
		);

		void create(
			std::string vertexShaderFile, 
			std::string fragmentShaderFile
		);

		void destroy();

		VkShaderModule createShaderModule(const std::vector<char>& code);

		GraphicsPipeline() = default;
};

#endif