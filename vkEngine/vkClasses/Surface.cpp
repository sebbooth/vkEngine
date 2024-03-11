#include "Surface.h"

Surface::Surface(std::shared_ptr<Instance> instanceObj, GLFWwindow* window)
{
    this->window = window;
    this->instanceObj = instanceObj;
    if (glfwCreateWindowSurface(instanceObj->instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}
