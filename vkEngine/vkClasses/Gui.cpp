#include "Gui.h"

Gui::Gui(std::shared_ptr<DescriptorSets> p_DescriptorSets)
{
    this->p_DescriptorSets = p_DescriptorSets;
    this->p_DescriptorPool = p_DescriptorSets->p_DescriptorPool;
    this->p_FrameBuffers = p_DescriptorPool->p_FrameBuffers;
    this->p_CommandPool = p_FrameBuffers->p_CommandPool;
    this->p_GraphicsPipeline = p_CommandPool->p_GraphicsPipeline;
    this->p_DescriptorSetLayout = p_GraphicsPipeline->p_DescriptorSetLayout;
    this->p_RenderPass = p_DescriptorSetLayout->p_RenderPass;
    this->p_ImageViews = p_RenderPass->p_ImageViews;
    this->p_SwapChain = p_ImageViews->p_SwapChain;
    this->p_LogicalDevice = p_SwapChain->p_LogicalDevice;
    this->p_PhysicalDevice = p_LogicalDevice->p_PhysicalDevice;
    this->p_Surface = p_PhysicalDevice->p_Surface;
    this->p_Instance = p_Surface->p_Instance;

    this->depthEnabled = p_RenderPass->depthEnabled;
    this->wireFrameEnabled = p_LogicalDevice->wireFrameEnabled;
    this->samplerEnabled = p_DescriptorSetLayout->samplerEnabled;
    this->msaaEnabled = p_PhysicalDevice->msaaEnabled;
    this->displayNormalsEnabled = p_GraphicsPipeline->displayNormals;
    this->cullBackFacesEnabled = p_GraphicsPipeline->cullBackFaces;

    this->prevDepthEnabled = p_RenderPass->depthEnabled;
    this->prevWireFrameEnabled = p_LogicalDevice->wireFrameEnabled;
    this->prevSamplerEnabled = p_DescriptorSetLayout->samplerEnabled;
    this->prevMsaaEnabled = p_PhysicalDevice->msaaEnabled;
    this->prevDisplayNormalsEnabled = p_GraphicsPipeline->displayNormals;
    this->prevCullBackFacesEnabled = p_GraphicsPipeline->cullBackFaces;

}

void Gui::draw(VkCommandBuffer commandBuffer)
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    ImGui::Begin("Hello, world!");       

    ImGui::Checkbox("Depth Enabled", &depthEnabled);
    ImGui::Checkbox("Wireframe Enabled", &wireFrameEnabled);
    ImGui::Checkbox("Texture Enabled", &samplerEnabled);
    ImGui::Checkbox("MSAA Enabled", &msaaEnabled);
    ImGui::Checkbox("Display Normals", &displayNormalsEnabled);
    ImGui::Checkbox("Cull Back Faces", &cullBackFacesEnabled);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();

    //ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, NULL);
}

const bool Gui::hasChanged()
{
    if (prevDepthEnabled != depthEnabled) return true;
    if (prevMsaaEnabled != msaaEnabled) return true;
    if (prevSamplerEnabled != samplerEnabled) return true;
    if (prevWireFrameEnabled != wireFrameEnabled) return true;
    if (prevDisplayNormalsEnabled != displayNormalsEnabled) return true;
    if (prevCullBackFacesEnabled != cullBackFacesEnabled) return true;

    return false;
}

void Gui::init()
{
    this->depthEnabled = p_RenderPass->depthEnabled;
    this->wireFrameEnabled = p_LogicalDevice->wireFrameEnabled;
    this->samplerEnabled = p_DescriptorSetLayout->samplerEnabled;
    this->msaaEnabled = p_PhysicalDevice->msaaEnabled;
    this->displayNormalsEnabled = p_GraphicsPipeline->displayNormals;
    this->cullBackFacesEnabled = p_GraphicsPipeline->cullBackFaces;

    this->prevDepthEnabled = p_RenderPass->depthEnabled;
    this->prevWireFrameEnabled = p_LogicalDevice->wireFrameEnabled;
    this->prevSamplerEnabled = p_DescriptorSetLayout->samplerEnabled;
    this->prevMsaaEnabled = p_PhysicalDevice->msaaEnabled;
    this->prevDisplayNormalsEnabled = p_GraphicsPipeline->displayNormals;
    this->prevCullBackFacesEnabled = p_GraphicsPipeline->cullBackFaces;

    ImGui::CreateContext();
    //ImGui::GetIO().ConfigFlags |= ImGUIConfigFlags_doc;
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForVulkan(p_Surface->window, true);


    ImGui_ImplVulkan_InitInfo info{};
    info.Instance = p_Instance->instance;
    info.PhysicalDevice = p_PhysicalDevice->physicalDevice;
    info.Device = p_LogicalDevice->device;

    info.QueueFamily = p_PhysicalDevice->graphicsFamily;
    info.Queue = p_LogicalDevice->graphicsQueue;

    info.PipelineCache = g_PipelineCache;
    info.DescriptorPool = p_DescriptorPool->descriptorPool;
    info.RenderPass = p_RenderPass->renderPass;
    info.Subpass = 0;

    info.MinImageCount = p_FrameBuffers->MAX_FRAMES_IN_FLIGHT;
    info.ImageCount = p_FrameBuffers->MAX_FRAMES_IN_FLIGHT;
    info.MSAASamples = p_PhysicalDevice->msaaSamples;

    info.Allocator = p_Instance->allocator;
    info.MinAllocationSize = 1024 * 1024;
    info.CheckVkResultFn = check_vk_result;

    info.UseDynamicRendering = false;




    ImGui_ImplVulkan_Init(&info);

    //VkCommandBuffer commandBuffer = p_CommandPool->beginSingleTimeCommands();

    ImGui_ImplVulkan_CreateFontsTexture();

    //p_CommandPool->endSingleTimeCommands(commandBuffer);

    vkDeviceWaitIdle(p_LogicalDevice->device);

    //ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void Gui::destroy()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
