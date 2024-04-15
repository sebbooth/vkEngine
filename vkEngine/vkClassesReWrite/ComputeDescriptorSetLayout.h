#pragma once

#ifndef COMPUTEDESCRIPTORSETLAYOUT_H
#define COMPUTEDESCRIPTORSETLAYOUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

class ComputeDescriptorSetLayout
{
public:
	VkDescriptorSetLayout descriptorSetLayout{};

private:
	VkDevice m_Device;
	std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings{};
	uint32_t m_Binding = 0;

public:
	ComputeDescriptorSetLayout(VkDevice device);

	uint32_t bindUniformBuffer();
	uint32_t bindStorageBuffer();
	uint32_t bindStorageImage();

	void create();

	void destroy();
};

#endif // !COMPUTEDESCRIPTORSETLAYOUT_H