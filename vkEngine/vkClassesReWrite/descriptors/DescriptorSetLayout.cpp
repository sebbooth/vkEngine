#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(
    VkDevice device
)
{
    m_Device = device;
}

uint32_t DescriptorSetLayout::bindVertUniformBuffer()
{
    VkDescriptorSetLayoutBinding uniformBufferLayoutBinding;
    uniformBufferLayoutBinding.binding = m_Binding;
    uniformBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformBufferLayoutBinding.descriptorCount = 1;
    uniformBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uniformBufferLayoutBinding.pImmutableSamplers = nullptr; // Optional

    m_LayoutBindings.push_back(uniformBufferLayoutBinding);

    return m_Binding++;
}

uint32_t DescriptorSetLayout::bindFragSampler()
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = m_Binding;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    m_LayoutBindings.push_back(samplerLayoutBinding);

    return m_Binding++;
}

uint32_t DescriptorSetLayout::bindCompUniformBuffer()
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

uint32_t DescriptorSetLayout::bindCompStorageBuffer()
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

uint32_t DescriptorSetLayout::bindCompStorageImage()
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

void DescriptorSetLayout::create()
{
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(m_LayoutBindings.size());
    layoutInfo.pBindings = m_LayoutBindings.data();

    if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void DescriptorSetLayout::destroy()
{
    vkDestroyDescriptorSetLayout(m_Device, descriptorSetLayout, nullptr);
}
