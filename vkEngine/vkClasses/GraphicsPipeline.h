#pragma once
#include "FileUtils.h"
#include "DescriptorSetLayout.h"

class GraphicsPipeline
{
public:
	std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutObj;

	VkPipeline graphicsPipeline{};
	VkPipelineLayout pipelineLayout{};

	GraphicsPipeline(std::shared_ptr<DescriptorSetLayout> descriptorSetLayoutObj);

	VkShaderModule createShaderModule(const std::vector<char>& code);
};

