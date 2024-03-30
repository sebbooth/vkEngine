#pragma once

#ifndef COMPUTEGRAPHICSPIPELINE_H
#define COMPUTEGRAPHICSPIPELINE_H

#include "FileUtils.h"
#include "DescriptorSetLayout.h"
#include "GraphicsPipeline.h"

class ComputeGraphicsPipeline : public GraphicsPipeline
{
public:
    ComputeGraphicsPipeline(std::shared_ptr<DescriptorSetLayout> p_DescriptorSetLayout);

	void create();
	void destroy();

	VkShaderModule createShaderModule(const std::vector<char>& code);
};

struct Particle {
    glm::vec2 position;
    glm::vec2 velocity;
    glm::vec4 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Particle);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Particle, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Particle, color);

        return attributeDescriptions;
    }
};

#endif