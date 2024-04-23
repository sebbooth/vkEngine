#include "Gui.h"

Gui::Gui(
    GLFWwindow* window,
    VkInstance instance,
    VkAllocationCallbacks* allocator,
    VkPhysicalDevice physicalDevice,
    VkSampleCountFlagBits msaaSamples,
    uint32_t graphicsFamily,
    VkDevice device,
    VkQueue graphicsQueue,
    VkDescriptorPool descriptorPool,
    VkRenderPass renderPass,
    std::shared_ptr<VkConfig> config
)
{
    m_Window = window;
    m_Instance = instance;
    m_Allocator = allocator;
    m_PhysicalDevice = physicalDevice;
    m_MsaaSamples = msaaSamples;
    m_GraphicsFamily = graphicsFamily;
    m_Device = device;
    m_GraphicsQueue = graphicsQueue;
    m_DescriptorPool = descriptorPool;
    m_RenderPass = renderPass;
    m_Config = config;
}

void Gui::draw(VkCommandBuffer commandBuffer)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        m_Config->cursorActive = false;
    }
    else {
        m_Config->cursorActive = true;
    }
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Debugger");


    static float accumulateFrameTime = 0;
    static float averageFrameTime = 0;
    static float frameTimes[256] = { 0 }; // Array to store frame times
    static int frameIndex = 0; // Index to keep track of the current frame
    static float updateInterval = 0.01f; // Update interval in seconds
    static float timeAccumulator = 0.0f; // Time accumulator
    

    // Add current frame time to the array when the time accumulator exceeds the update interval
    timeAccumulator += ImGui::GetIO().DeltaTime;
    if (timeAccumulator >= updateInterval) {
        frameIndex = (frameIndex + 1) % 256;
        accumulateFrameTime -= frameTimes[frameIndex];
        frameTimes[frameIndex] = ImGui::GetIO().DeltaTime * 1000.0f; // GetIO().DeltaTime gives the time between frames in seconds
        accumulateFrameTime += frameTimes[frameIndex];
        timeAccumulator -= updateInterval; // Subtract update interval from the accumulator
        averageFrameTime = accumulateFrameTime / 256;
    }

    if (ImGui::CollapsingHeader("Performance")) {
        ImGui::Text("Application average %.3f ms/frame (%5.1f FPS) (source: ImGui)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::Text("Application average %.3f ms/frame (%5.1f FPS) (source: GLFW)", m_Config->lastFrameTime, 1000.0f / m_Config->lastFrameTime);

        std::string averageText = "Average: " + std::to_string(averageFrameTime) + " ms";
        ImGui::PlotLines("Frame Times (ms)", frameTimes, IM_ARRAYSIZE(frameTimes), frameIndex, averageText.c_str(), 0.0f, 20.0f, ImVec2(0, 80));

    }
    
    if (ImGui::CollapsingHeader("Camera")) {
        ImGui::Text("%s: (%.3f, %.3f, %.3f)", "Cam Position", m_Config->ubo.camPos.x, m_Config->ubo.camPos.y, m_Config->ubo.camPos.z);
        ImGui::Text("%s: (%.3f, %.3f, %.3f)", "Cam Direction", m_Config->ubo.camDir.x, m_Config->ubo.camDir.y, m_Config->ubo.camDir.z);
        ImGui::Text("%s: (%.3f, %.3f, %.3f)", "Cam Up", m_Config->ubo.camUp.x, m_Config->ubo.camUp.y, m_Config->ubo.camUp.z);
        ImGui::SliderFloat("Focal Length", &m_Config->ubo.focalLength, 0.0f, 10.0f);
        ImGui::SliderFloat("Viewport Height", &m_Config->ubo.viewportHeight, 0.0f, 10.0f);
    }

    if (ImGui::CollapsingHeader("Controls")) {
        ImGui::SliderFloat("Rotation Speed", &m_Config->rotateSpeed, 0.0f, 0.03f);
        ImGui::SliderFloat("Movement Speed", &m_Config->moveSpeed, 0.0f, 0.50f);
    }

    if (ImGui::CollapsingHeader("Octree Generation")) {
        ImGui::SliderFloat("Rotation Speed", &m_Config->rotateSpeed, 0.0f, 0.03f);
        ImGui::SliderFloat("Movement Speed", &m_Config->moveSpeed, 0.0f, 0.50f);
    }

    static const char* items[] = { "1/1", "1/2", "1/4", "1/8", "1/16", "1/32" };
    int selectedResolution = std::log2(m_Config->downScaleFactor);
    if (ImGui::CollapsingHeader("Render Settings")) {
        if (ImGui::Combo("Resolution", &selectedResolution, items, IM_ARRAYSIZE(items))) {
            m_Config->downScaleFactor = std::pow(2, selectedResolution);
            m_Config->newSwapChainNeeded = true;
        }
    }



    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, NULL);
}



const bool Gui::hasChanged()
{
    return false;
}

void Gui::init()
{
    ImGui::CreateContext();
    //ImGui::GetIO().ConfigFlags |= ImGUIConfigFlags_doc;
    ImGuiIO& io = ImGui::GetIO(); (void)io;
   
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForVulkan(m_Window, true);

    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = m_Instance;
    info.PhysicalDevice = m_PhysicalDevice;
    info.Device = m_Device;

    info.QueueFamily = m_GraphicsFamily;
    info.Queue = m_GraphicsQueue;

    info.PipelineCache = g_PipelineCache;
    info.DescriptorPool = m_DescriptorPool;
    info.RenderPass = m_RenderPass;
    info.Subpass = 0;

    info.MinImageCount = m_Config->maxFramesInFlight;
    info.ImageCount = m_Config->maxFramesInFlight;
    info.MSAASamples = m_MsaaSamples;

    info.Allocator = m_Allocator;
    info.MinAllocationSize = 1024 * 1024;
    info.CheckVkResultFn = check_vk_result;

    info.UseDynamicRendering = false;

    ImGui_ImplVulkan_Init(&info);

    //VkCommandBuffer commandBuffer = p_CommandPool->beginSingleTimeCommands();

    //ImGui_ImplVulkan_CreateFontsTexture();

    //p_CommandPool->endSingleTimeCommands(commandBuffer);

    vkDeviceWaitIdle(m_Device);

    //ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Gui::reInit(VkDescriptorPool descriptorPool)
{
    m_DescriptorPool = descriptorPool;

    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = m_Instance;
    info.PhysicalDevice = m_PhysicalDevice;
    info.Device = m_Device;

    info.QueueFamily = m_GraphicsFamily;
    info.Queue = m_GraphicsQueue;

    info.PipelineCache = g_PipelineCache;
    info.DescriptorPool = m_DescriptorPool;
    info.RenderPass = m_RenderPass;
    info.Subpass = 0;

    info.MinImageCount = m_Config->maxFramesInFlight;
    info.ImageCount = m_Config->maxFramesInFlight;
    info.MSAASamples = m_MsaaSamples;

    info.Allocator = m_Allocator;
    info.MinAllocationSize = 1024 * 1024;
    info.CheckVkResultFn = check_vk_result;

    info.UseDynamicRendering = false;

    ImGui_ImplVulkan_Init(&info);
    vkDeviceWaitIdle(m_Device);
}

void Gui::vulkanShutdown()
{
    ImGui_ImplVulkan_Shutdown();
}

void Gui::destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
