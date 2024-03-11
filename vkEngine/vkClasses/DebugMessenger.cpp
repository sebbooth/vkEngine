#include "DebugMessenger.h"

DebugMessenger::DebugMessenger(std::shared_ptr<Instance> instanceObj)
{
    if (!instanceObj->enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instanceObj->instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

