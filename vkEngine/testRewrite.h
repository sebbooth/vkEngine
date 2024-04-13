#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION

#include "VkClassesReWrite.h"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;
const std::string MODEL_PATH = "assets/models/viking_room/viking_room.obj";
const std::string TEXTURE_PATH = "assets/models/viking_room/viking_room.png";
const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const bool enableValidationLayers = true;

/*
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif
*/

class testRewrite {
public:
    void run() {
        initWindow();
        initVulkan();
        cleanup();
    }

private:
    GLFWwindow* window = nullptr;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    std::shared_ptr<Instance> p_Instance;
    std::shared_ptr<Surface> p_Surface;

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    }

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto app = reinterpret_cast<testRewrite*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    }

    void initVulkan() {
        p_Instance = std::make_shared<Instance>(enableValidationLayers, validationLayers, deviceExtensions);
        p_Instance->create();

        p_Surface = std::make_shared<Surface>(p_Instance->instance, window);
    }

    void cleanup() {
        p_Surface->destroy();

        p_Instance->DestroyDebugUtilsMessengerEXT(nullptr);
        p_Instance->destroy();
    }
};
