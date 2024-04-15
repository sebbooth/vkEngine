#include "ComputeDescriptorSetLayout.h"

ComputeDescriptorSetLayout::ComputeDescriptorSetLayout(VkDevice device)
{
	m_Device = device;
}

uint32_t ComputeDescriptorSetLayout::bindUniformBuffer()
{
	VkDescriptorSetLayoutBinding uniformBufferLayoutBinding;
	uniformBufferLayoutBinding.binding = m_Binding;
	uniformBufferLayoutBinding.descriptorCount = 1;
	uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniformBufferLayoutBinding.pImmutableSamplers = nullptr;
	uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	m_LayoutBindings.push_back(uniformBufferLayoutBinding);

	return m_Binding++;
}

uint32_t ComputeDescriptorSetLayout::bindStorageBuffer()
{
	VkDescriptorSetLayoutBinding storageBufferLayoutBinding;
	storageBufferLayoutBinding.binding = m_Binding;
	storageBufferLayoutBinding.descriptorCount = 1;
	storageBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	storageBufferLayoutBinding.pImmutableSamplers = nullptr;
	storageBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	m_LayoutBindings.push_back(storageBufferLayoutBinding);

	return m_Binding++;
}

uint32_t ComputeDescriptorSetLayout::bindStorageImage()
{
	VkDescriptorSetLayoutBinding storageImageLayoutBinding;
	storageImageLayoutBinding.binding = m_Binding;
	storageImageLayoutBinding.descriptorCount = 1;
	storageImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	storageImageLayoutBinding.pImmutableSamplers = nullptr;
	storageImageLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	m_LayoutBindings.push_back(storageImageLayoutBinding);

	return m_Binding++;
}

void ComputeDescriptorSetLayout::create()
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(m_LayoutBindings.size());
	layoutInfo.pBindings = m_LayoutBindings.data();

	if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor set layout!");
	}
}

void ComputeDescriptorSetLayout::destroy()
{
	vkDestroyDescriptorSetLayout(m_Device, descriptorSetLayout, nullptr);
}
