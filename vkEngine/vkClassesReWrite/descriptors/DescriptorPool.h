#pragma once
#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "initialization/VkConfig.h"

class DescriptorPool
{
public:
	VkDescriptorPool descriptorPool;

private:
	VkDevice m_Device;
	std::shared_ptr<VkConfig> m_Config;

	std::vector<VkDescriptorPoolSize> m_PoolSizes;

public:
	DescriptorPool(
		VkDevice device,
		std::shared_ptr<VkConfig> config
	);
	void bindUniformBuffer(uint32_t binding, uint32_t descriptorCount);
	void bindStorageBuffer(uint32_t binding, uint32_t descriptorCount);
	void bindStorageImage(uint32_t binding, uint32_t descriptorCount);
	void create();
	void destroy();
};

#endif // !DESCRIPTORPOOL_H
