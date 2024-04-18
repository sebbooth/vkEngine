#include "DescriptorPool.h"

DescriptorPool::DescriptorPool(VkDevice device, std::shared_ptr<VkConfig> config)
{
	m_Device = device;
	m_Config = config;
}

void DescriptorPool::bindUniformBuffer(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
}

void DescriptorPool::bindStorageBuffer(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
}

void DescriptorPool::bindStorageImage(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
}

void DescriptorPool::create()
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 3;
	poolInfo.pPoolSizes = m_PoolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_Config->MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorPool::destroy()
{
	vkDestroyDescriptorPool(m_Device, descriptorPool, nullptr);
}