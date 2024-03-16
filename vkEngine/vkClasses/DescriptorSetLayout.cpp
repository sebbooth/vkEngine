#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(std::shared_ptr<RenderPass> p_RenderPass)
{
    this->p_RenderPass = p_RenderPass;
    this->p_ImageViews = p_RenderPass->p_ImageViews;
    this->p_SwapChain = p_ImageViews->p_SwapChain;
    this->p_LogicalDevice = p_SwapChain->p_LogicalDevice;
    this->p_PhysicalDevice = p_LogicalDevice->p_PhysicalDevice;
    this->p_Surface = p_PhysicalDevice->p_Surface;
    this->p_Instance = p_Surface->p_Instance;  
}

void DescriptorSetLayout::create()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    uint32_t bindingCount = 0;

    //= { uboLayoutBinding, samplerLayoutBinding };

    if (uboEnabled) {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = bindingCount++;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

        bindings.push_back(uboLayoutBinding);
    }
    
    if (samplerEnabled) {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
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

    if (vkCreateDescriptorSetLayout(p_LogicalDevice->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
}

void DescriptorSetLayout::destroy()
{
    vkDestroyDescriptorSetLayout(p_LogicalDevice->device, descriptorSetLayout, nullptr);
}
