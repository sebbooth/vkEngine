#include "DescriptorSets.h"

DescriptorSets::DescriptorSets(
	VkDevice device, 
	VkDescriptorSetLayout descriptorSetLayout,
	VkDescriptorPool descriptorPool,
	std::shared_ptr<VkConfig> config
)
{
	m_Device = device;
	m_DescriptorSetLayout = descriptorSetLayout;
	m_DescriptorPool = descriptorPool;
	m_Config = config;

	descriptorSets.resize(m_Config->MAX_FRAMES_IN_FLIGHT);
	m_DescriptorWrites.resize(m_Config->MAX_FRAMES_IN_FLIGHT);

	std::vector<VkDescriptorSetLayout> layouts(m_Config->MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Config->MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(m_Device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

}

void DescriptorSets::create()
{
	
	for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
		vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(m_DescriptorWrites[i].size()), m_DescriptorWrites[i].data(), 0, nullptr);
	}
}

void DescriptorSets::bindUniformBuffer(uint32_t binding, std::vector<VkBuffer> uniformBuffers, VkDeviceSize size)
{
	size_t j = m_UniformBufferInfoSets.size();
	m_UniformBufferInfoSets.resize(j + 1);
	m_UniformBufferInfoSets[j].resize(m_Config->MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
		if (binding >= m_DescriptorWrites[i].size()) {
			m_DescriptorWrites[i].resize(static_cast<size_t>(binding) + 1);
		}

		m_UniformBufferInfoSets[j][i].buffer = uniformBuffers[i];
		m_UniformBufferInfoSets[j][i].offset = 0;
		m_UniformBufferInfoSets[j][i].range = size;

		m_DescriptorWrites[i][binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_DescriptorWrites[i][binding].dstSet = descriptorSets[i];
		m_DescriptorWrites[i][binding].dstBinding = binding;
		m_DescriptorWrites[i][binding].dstArrayElement = 0;
		m_DescriptorWrites[i][binding].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		m_DescriptorWrites[i][binding].descriptorCount = 1;
		m_DescriptorWrites[i][binding].pBufferInfo = &m_UniformBufferInfoSets[j][i];
	}

}

void DescriptorSets::bindStorageBuffer(uint32_t binding, std::vector<VkBuffer> storageBuffers, VkDeviceSize size)
{
	size_t j = m_StorageBufferInfoSets.size();
	m_StorageBufferInfoSets.resize(j + 1);
	m_StorageBufferInfoSets[j].resize(m_Config->MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
		if (binding >= m_DescriptorWrites[i].size()) {
			m_DescriptorWrites[i].resize(static_cast<size_t>(binding) + 1);
		}

		m_StorageBufferInfoSets[j][i].buffer = storageBuffers[i];
		m_StorageBufferInfoSets[j][i].offset = 0;
		m_StorageBufferInfoSets[j][i].range = size;

		m_DescriptorWrites[i][binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_DescriptorWrites[i][binding].dstSet = descriptorSets[i];
		m_DescriptorWrites[i][binding].dstBinding = binding;
		m_DescriptorWrites[i][binding].dstArrayElement = 0;
		m_DescriptorWrites[i][binding].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		m_DescriptorWrites[i][binding].descriptorCount = 1;
		m_DescriptorWrites[i][binding].pBufferInfo = &m_StorageBufferInfoSets[j][i];
	}
}

void DescriptorSets::bindStorageImage(uint32_t binding, std::vector<VkImageView> storageImageViews)
{
	size_t j = m_StorageImageInfoSets.size();
	m_StorageImageInfoSets.resize(j + 1);
	m_StorageImageInfoSets[j].resize(m_Config->MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
		if (binding >= m_DescriptorWrites[i].size()) {
			m_DescriptorWrites[i].resize(static_cast<size_t>(binding) + 1);
		}

		m_StorageImageInfoSets[j][i].imageView = storageImageViews[i];
		m_StorageImageInfoSets[j][i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		m_DescriptorWrites[i][binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_DescriptorWrites[i][binding].dstSet = descriptorSets[i];
		m_DescriptorWrites[i][binding].dstBinding = binding;
		m_DescriptorWrites[i][binding].dstArrayElement = 0;
		m_DescriptorWrites[i][binding].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		m_DescriptorWrites[i][binding].descriptorCount = 1;
		m_DescriptorWrites[i][binding].pImageInfo = &m_StorageImageInfoSets[j][i];
	}
}

void DescriptorSets::bindSampler(uint32_t binding, std::vector<VkImageView> imageViews, std::vector<VkSampler> imageSamplers)
{
	size_t j = m_StorageImageInfoSets.size();
	m_StorageImageInfoSets.resize(j + 1);
	m_StorageImageInfoSets[j].resize(m_Config->MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < m_Config->MAX_FRAMES_IN_FLIGHT; i++) {
		if (binding >= m_DescriptorWrites[i].size()) {
			m_DescriptorWrites[i].resize(static_cast<size_t>(binding) + 1);
		}

		m_StorageImageInfoSets[j][i].sampler = imageSamplers[i];
		m_StorageImageInfoSets[j][i].imageView = imageViews[i];
		m_StorageImageInfoSets[j][i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;

		m_DescriptorWrites[i][binding].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		m_DescriptorWrites[i][binding].dstSet = descriptorSets[i];
		m_DescriptorWrites[i][binding].dstBinding = binding;
		m_DescriptorWrites[i][binding].dstArrayElement = 0;
		m_DescriptorWrites[i][binding].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		m_DescriptorWrites[i][binding].descriptorCount = 1;
		m_DescriptorWrites[i][binding].pImageInfo = &m_StorageImageInfoSets[j][i];
	}
}
