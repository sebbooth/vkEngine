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
	m_PoolSizeCount++;
}

void DescriptorPool::bindSampler(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
	m_PoolSizeCount++;
}

void DescriptorPool::bindStorageBuffer(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
	m_PoolSizeCount++;
}

void DescriptorPool::bindStorageImage(uint32_t binding, uint32_t descriptorCount)
{
	if (binding >= m_PoolSizes.size()) {
		m_PoolSizes.resize(static_cast<size_t>(binding) + 1);
	}

	m_PoolSizes[binding].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	m_PoolSizes[binding].descriptorCount = descriptorCount;
	m_PoolSizeCount++;
}

void DescriptorPool::create()
{
	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = m_PoolSizeCount;
	poolInfo.pPoolSizes = m_PoolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_Config->maxFramesInFlight);

	if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void DescriptorPool::destroy()
{
	m_PoolSizes.resize(0);
	m_PoolSizeCount = 0;
	vkDestroyDescriptorPool(m_Device, descriptorPool, nullptr);
}
