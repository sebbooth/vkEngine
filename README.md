# vkEngine 
(name for now cause naming things is hard)

## 2024-03-11

Plan with this project is to make a voxel engine (and just to learn Vulkan).

Completed most of https://vulkan-tutorial.com/ a couple nights ago, and now trying to abstract all of the Vulkan objects and initialization code into classes. I could of course have just used Vulkan-Hpp (https://github.com/KhronosGroup/Vulkan-Hpp) or another set of C++ bindings, but I wanted to do it myself to gain a better understanding of what all the VK objects do and how they interact. So far the class implementation are very rigid, not allowing for much customization of the pipelines, but plan is to just add functionality for flexibility as the project progresses.
