# vkEngine 
(name for now cause naming things is hard)


## 2024-03-16

Finished abstracting a basic rasterization application
into classes a couple days ago, have since been trying
to inject some flexibility into the configuration, 
rather than everything being hard-coded. Tackling a 
couple bugs at the moment that seem to be due to some
sort of misalignment between attachments at different stages.
(bug would only occur in release mode at first, but my attempts
at fixing it have caused it to spread to debug as well
in certain configurations).

Hoping to add ImGUI next to test adjusting various
configuration settings live.

## 2024-03-11

Plan with this project is to make a voxel engine 
(and just to learn Vulkan).

Completed most of https://vulkan-tutorial.com/ a 
couple nights ago, and now trying to abstract 
all of the Vulkan objects and initialization 
code into classes. I could of course have just 
used Vulkan-Hpp (https://github.com/KhronosGroup/Vulkan-Hpp) 
or another set of C++ bindings, but I wanted 
to do it myself to gain a better understanding 
of what all the VK objects do and how they 
interact. So far the class implementation are 
very rigid, not allowing for much customization 
of the pipelines, but plan is to just add 
functionality for flexibility as the project 
progresses.
