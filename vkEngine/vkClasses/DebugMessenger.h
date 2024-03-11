#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <memory>

#include "Instance.h"
#include "DebugUtils.h"


class DebugMessenger
{
public:
	VkDebugUtilsMessengerEXT debugMessenger{};

	DebugMessenger(std::shared_ptr<Instance> instanceObj);
};

