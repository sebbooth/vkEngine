#pragma once
#ifndef DESCRIPTORSETS_H
#define DESCRIPTORSETS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "initialization/VkConfig.h"
#include "descriptors/DescriptorSetLayout.h"
#include "descriptors/DescriptorPool.h"


class DescriptorSets
{
public:
	std::vector<VkDescriptorSet> descriptorSets;

private:
	VkDevice m_Device;
	std::shared_ptr<DescriptorSetLayout> mp_DescriptorSetLayout;
	std::shared_ptr<DescriptorPool> mp_DescriptorPool;
	std::shared_ptr<VkConfig> m_Config;

	std::vector<std::vector<VkWriteDescriptorSet>> m_DescriptorWrites;

	std::vector<std::vector<VkDescriptorBufferInfo>> m_UniformBufferInfoSets;	// Needed so that pointers don't get tossed after bindings are finished
	std::vector<std::vector<VkDescriptorBufferInfo>> m_StorageBufferInfoSets;
	std::vector<std::vector<VkDescriptorImageInfo>> m_StorageImageInfoSets;

public:
	DescriptorSets(
		VkDevice device,
		std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout,
		std::shared_ptr<DescriptorPool> p_DescriptorPool,
		std::shared_ptr<VkConfig> config
	);

	void create();
	void reInit();
	void destroy();

	void bindUniformBuffer(
		uint32_t binding, 
		std::vector<VkBuffer> uniformBuffers, 
		VkDeviceSize size
	);

	void bindStorageBuffer(
		uint32_t binding,
		std::vector<VkBuffer> storageBuffers,
		VkDeviceSize size
	);

	void bindStorageImage(
		uint32_t binding,
		std::vector<VkImageView> storageImageViews
	);

	void bindSampler(
		uint32_t binding, 
		std::vector<VkImageView> imageViews, 
		std::vector<VkSampler> imageSamplers
	);
};

#endif // !DESCRIPTORSETS_H
