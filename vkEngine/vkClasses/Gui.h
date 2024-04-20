#pragma once

#ifndef GUI_H
#define GUI_H

#include <memory>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

/*
#include "imconfig.h"
#include "imgui_tables.cpp"
#include "imgui_internal.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"
#include "imgui_impl_glfw.cpp"
#include "imgui_impl_vulkan.h"
*/

#include "DescriptorSets.h"

class Gui
{
public:
	std::shared_ptr<Instance> p_Instance;
	std::shared_ptr<Surface> p_Surface;
	std::shared_ptr<PhysicalDevice> p_PhysicalDevice;
	std::shared_ptr<LogicalDevice> p_LogicalDevice;
	std::shared_ptr<SwapChain> p_SwapChain;
	std::shared_ptr<ImageViews> p_ImageViews;
	std::shared_ptr<RenderPass> p_RenderPass;
	std::shared_ptr<DescriptorSetLayout> p_GraphicsDescriptorSetLayout;
	std::shared_ptr<GraphicsPipeline> p_GraphicsPipeline;
	std::shared_ptr<CommandPool> p_CommandPool;
	std::shared_ptr<FrameBuffers> p_FrameBuffers;
	std::shared_ptr<DescriptorPool> p_DescriptorPool;
	std::shared_ptr<DescriptorSets> p_DescriptorSets;

	bool depthEnabled;
	bool wireFrameEnabled;
	bool samplerEnabled;
	bool msaaEnabled;
	bool displayNormalsEnabled;
	bool cullBackFacesEnabled;

	bool prevDepthEnabled;
	bool prevWireFrameEnabled;
	bool prevSamplerEnabled;
	bool prevMsaaEnabled;
	bool prevDisplayNormalsEnabled;
	bool prevCullBackFacesEnabled;

	Gui(std::shared_ptr<DescriptorSets> p_DescriptorSets);

	void draw(VkCommandBuffer commandBuffer);

	const bool hasChanged();

	void init();

	void destroy();
};

#endif

static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;

