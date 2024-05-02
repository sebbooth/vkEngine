C:/VulkanSDK/1.3.275.0/Bin/glslc.exe simplerRaytracer.comp -o raytrace_comp.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe octreeRaycaster.comp -o raycast_comp.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe oldOctreeRaycaster.comp -o old_raycast_comp.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe bitmaskOctree.comp -o bitmask.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe depthPass.comp -o depthPass.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe voxelTraversalVisualizeOctree.comp -o raycast_vis_comp.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe shadowRays.comp -o shadowRays.spv

C:/VulkanSDK/1.3.275.0/Bin/glslc.exe simple_shader.vert -o simple_vert.spv
C:/VulkanSDK/1.3.275.0/Bin/glslc.exe simple_shader.frag -o simple_frag.spv


pause