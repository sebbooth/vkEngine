#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "DebugUtils.h"


class Instance
{
public:
	VkInstance instance{};
	bool enableValidationLayers;
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;
	
	Instance(bool enableValidationLayers, std::vector<const char*> validationLayers, std::vector<const char*> deviceExtensions);

private:
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
};
