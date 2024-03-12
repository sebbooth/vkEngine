#pragma once
#include "Instance.h"
#include "DebugUtils.h"


class DebugMessenger
{
public:
	VkDebugUtilsMessengerEXT debugMessenger{};

	DebugMessenger(std::shared_ptr<Instance> instanceObj);
};

