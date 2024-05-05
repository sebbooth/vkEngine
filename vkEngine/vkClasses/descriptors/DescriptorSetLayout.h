#pragma once

#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

class DescriptorSetLayout
{
	public:
		VkDescriptorSetLayout descriptorSetLayout{};

	private:
		VkDevice m_Device;
		std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings{};
		uint32_t m_Binding = 0;

	public:
		DescriptorSetLayout(
			VkDevice device
		);

		uint32_t bindVertUniformBuffer();
		uint32_t bindFragSampler();
		uint32_t bindCompUniformBuffer();
		uint32_t bindCompStorageBuffer();
		uint32_t bindCompStorageImage();

		void create();
		void destroy();

		DescriptorSetLayout() = default;
};

#endif