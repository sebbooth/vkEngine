#pragma once

#ifndef GUI_H
#define GUI_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>
#include <iostream>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#include "initialization/VkConfig.h"

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

class Gui
{
public:
	
private:
	GLFWwindow* m_Window;
	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice;
	VkSampleCountFlagBits m_MsaaSamples;
	uint32_t m_GraphicsFamily;
	VkDevice m_Device;
	VkQueue m_GraphicsQueue;
	VkDescriptorPool m_DescriptorPool;
	VkRenderPass m_RenderPass;
	VkAllocationCallbacks* m_Allocator;
	std::shared_ptr<VkConfig> m_Config;

	bool performanceDropdown = true;
	bool cameraDropdown = true;

public:
	Gui(
		GLFWwindow* window,
		VkInstance instance,
		VkAllocationCallbacks* allocator,
		VkPhysicalDevice physicalDevice,
		VkSampleCountFlagBits msaaSamples,
		uint32_t graphicsFamily,
		VkDevice device,
		VkQueue graphicsQueue,
		VkDescriptorPool descriptorPool,
		VkRenderPass renderPass,
		std::shared_ptr<VkConfig> config
	);

	void draw(VkCommandBuffer commandBuffer);
	const bool hasChanged();
	void init();
	void reInit(VkDescriptorPool descriptorPool);
	void vulkanShutdown();
	void destroy();
};


static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

static VkPipelineCache g_PipelineCache = VK_NULL_HANDLE;

#endif



