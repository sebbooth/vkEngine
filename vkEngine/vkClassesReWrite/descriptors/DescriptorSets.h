#pragma once
#ifndef DESCRIPTORSETS_H
#define DESCRIPTORSETS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include <stdexcept>

#include "initialization/VkConfig.h"

class DescriptorSets
{
public:
	std::vector<VkDescriptorSet> descriptorSets;

private:
	VkDevice m_Device;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkDescriptorPool m_DescriptorPool;
	std::shared_ptr<VkConfig> m_Config;

	std::vector<std::vector<VkWriteDescriptorSet>> m_DescriptorWrites;

	std::vector<std::vector<VkDescriptorBufferInfo>> m_UniformBufferInfoSets;	// Needed so that pointers don't get tossed after bindings are finished
	std::vector<std::vector<VkDescriptorBufferInfo>> m_StorageBufferInfoSets;
	std::vector<std::vector<VkDescriptorImageInfo>> m_StorageImageInfoSets;

public:
	DescriptorSets(
		VkDevice device,
		VkDescriptorSetLayout descriptorSetLayout,
		VkDescriptorPool descriptorPool,
		std::shared_ptr<VkConfig> config
	);

	void create();

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
