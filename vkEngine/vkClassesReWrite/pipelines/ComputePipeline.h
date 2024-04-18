#pragma once

#ifndef COMPUTEPIPELINE_H
#define COMPUTEPIPELINE_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "FileUtils.h"

class ComputePipeline
{
	public:
		VkPipeline computePipeline{};
		VkPipelineLayout computePipelineLayout{};

	private:
		VkDevice m_Device{};
		VkDescriptorSetLayout m_ComputeDescriptorSetLayout{};

	public:
		ComputePipeline(
			VkDevice device,
			VkDescriptorSetLayout computeDescriptorSetLayout
		);

		void create(std::string computeShaderFile);
		void destroy();

	private:
		VkShaderModule createShaderModule(const std::vector<char>& code);
};

#endif