#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

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

