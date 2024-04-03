#pragma once

#ifndef DESCRIPTORSETLAYOUTRAYS_H
#define DESCRIPTORSETLAYOUTRAYS_H

#include "RenderPass.h"
#include "DescriptorSetLayout.h"


class DescriptorSetLayoutRays : public DescriptorSetLayout
{
public:
	DescriptorSetLayoutRays(std::shared_ptr<RenderPass> p_RenderPass);
	void create();
	void destroy();
};

#endif