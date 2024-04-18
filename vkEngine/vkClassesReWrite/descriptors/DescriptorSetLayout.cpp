#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(
    VkDevice device,
    std::shared_ptr<VkConfig> config
)
{
    m_Device = device;
    m_Config = config;
}

void DescriptorSetLayout::create()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    uint32_t bindingCount = 0;

    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    if (m_Config->uboEnabled) {
        uboLayoutBinding.binding = bindingCount++;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        bindings.push_back(uboLayoutBinding);
    }

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    if (m_Config->samplerEnabled || m_Config->guiEnabled) {
        samplerLayoutBinding.binding = bindingCount++;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

        bindings.push_back(samplerLayoutBinding);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
}

void DescriptorSetLayout::destroy()
{
    vkDestroyDescriptorSetLayout(m_Device, descriptorSetLayout, nullptr);
}
