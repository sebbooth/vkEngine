#pragma once
#ifndef SAMPLER_H
#define SAMPLER_H


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "initialization/VkConfig.h"

class Sampler
{
public:
	std::vector<VkSampler> imageSamplers;

private:
	VkPhysicalDevice m_PhysicalDevice;
	VkDevice m_Device;
	std::shared_ptr<VkConfig> m_Config;

public:
	Sampler(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		std::shared_ptr<VkConfig> config
	);

	void create();
	void destroy();
};

#endif // !SAMPLER_H