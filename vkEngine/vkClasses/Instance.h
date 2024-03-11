#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <stdexcept>
#include <iostream>

#include "VkUtils.h"


class Instance
{
public:
	VkInstance instance{};
	Instance(bool enableValidationLayers, std::vector<const char*> validationLayers, std::vector<const char*> deviceExtensions);

private:
	bool enableValidationLayers;
	std::vector<const char*> validationLayers;
	std::vector<const char*> deviceExtensions;

	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();
};
