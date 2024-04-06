#pragma once

#ifndef GRAPHICSPIPELINE_H
#define GRAPHICSPIPELINE_H

#include "FileUtils.h"
#include "DescriptorSetLayout.h"

class GraphicsPipeline
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

	VkPipeline graphicsPipeline{};
	VkPipelineLayout pipelineLayout{};

	bool displayNormals = false;
	bool cullBackFaces = true;
	bool simpleShader = false;

	GraphicsPipeline(std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout);

	void create();
	void destroy();

	VkShaderModule createShaderModule(const std::vector<char>& code);

	GraphicsPipeline() = default;
};

#endif