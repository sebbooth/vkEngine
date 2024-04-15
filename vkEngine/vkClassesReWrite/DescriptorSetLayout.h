#pragma once

#ifndef DESCRIPTORSETLAYOUT_H
#define DESCRIPTORSETLAYOUT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "RenderingSettings.h"

class DescriptorSetLayout
{
	public:
		VkDescriptorSetLayout descriptorSetLayout{};

	private:
		VkDevice m_Device;
		std::shared_ptr<RenderingSettings> m_RS;

	public:
		DescriptorSetLayout(
			VkDevice device,
			std::shared_ptr<RenderingSettings> RS
		);
		void create();
		void destroy();

		DescriptorSetLayout() = default;
};

#endif