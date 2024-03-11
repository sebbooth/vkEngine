#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <VkUtils.h>

class DebugMessenger
{
public:
	VkDebugUtilsMessengerEXT debugMessenger{};

	DebugMessenger(bool enableValidationLayers, VkInstance instance);
};

