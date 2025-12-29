#include <iostream>
#include <vector>
#include "renderer/Vulkan/core/GEVulkan.h"


void VulkanRenderer::CreateInstance()
{
    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Engine Instance";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0 , 0);
    appInfo.pEngineName = "Game";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0 ,0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    // TODO : Refactor extension concat to a better solution
    //        Preferably by creating a new class called ExtensionManager
    //        Need to figure out how to manage data flow between glfwGetRequiredExtensions and vulkanExtensions that we need

    std::vector<const char *> Extensions(requiredExtensions.extensions, requiredExtensions.extensions + requiredExtensions.count);
    if(vkValidationLayer.IsValidationLayerEnabled())
    {    
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = Extensions.size();
    createInfo.ppEnabledExtensionNames = Extensions.data();

    if(vkValidationLayer.IsValidationLayerEnabled() && !vkValidationLayer.CheckForValidationSupport())
    {
        createInfo.enabledLayerCount = vkValidationLayer.GetValidationLayersCount();
        createInfo.ppEnabledLayerNames = vkValidationLayer.GetValidationLayers();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS)
    {
        std::runtime_error("Unable to create Vulkan Instance");
    }

    vkValidationLayer.SetupDebugMessenger(vkInstance);
}

void VulkanRenderer::RecordCommands(uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(frameContext->GetCommandBuffer(), &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPass->GetRenderPass();
	renderPassInfo.framebuffer = vkSwapChain->GetFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vkSwapChain->GetSwapChainExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(frameContext->GetCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(frameContext->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetGraphicsPipeline());
	vkCmdSetViewport(frameContext->GetCommandBuffer(), 0, 1, &frameContext->GetViewport());
	vkCmdSetScissor(frameContext->GetCommandBuffer(), 0, 1, &frameContext->GetScissor());
	vkCmdDraw(frameContext->GetCommandBuffer(), 3, 1, 0, 0);
	vkCmdEndRenderPass(frameContext->GetCommandBuffer());

	if (vkEndCommandBuffer(frameContext->GetCommandBuffer()) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

VulkanRenderer::VulkanRenderer(WindowCreator* window) : Renderer(window)
{
	
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Initialize()
{
	CreateInstance();

    vkSurfaceView = std::make_unique<GEVulkanSurfaceView>(vkInstance);
	vkSurfaceView->CreateSurface(m_window);

    vkPhysicalDevice = std::make_unique<GEVulkanPhysicalDevice>(*vkSurfaceView);
	vkPhysicalDevice->pickPhysicalDevice(vkInstance);

	vkLogicalDevice = std::make_unique<GEVulkanLogicalDevice>(vkPhysicalDevice.get());
	vkLogicalDevice->createLogicalDevice();

	GEFramebufferSize fbSize = m_window->getFramebufferSize();
	vkSwapChain = std::make_unique<GEVulkanSwapChain>(vkPhysicalDevice.get(), vkLogicalDevice.get() , vkSurfaceView.get(), fbSize.width, fbSize.height);
	vkSwapChain->CreateSwapChain();

	vkRenderPass = std::make_unique<GEVulkanRenderPass>(vkLogicalDevice->getVkDevice(), vkSwapChain->GetSwapChainImageFormat());
	vkRenderPass->CreateRenderPass();

	frameContext = std::make_unique<GEVulkanFrameContext>(fbSize.width, fbSize.height);
	frameContext->CreateFrameContext();

	vkPipeline = std::make_unique<GEVulkanPipeline>(vkLogicalDevice->getVkDevice(), *frameContext, *vkRenderPass);
	vkPipeline->CreatePipeline();

	vkSwapChain->SetRenderPass(vkRenderPass.get());
	vkSwapChain->CreateFramebuffers();

	vkCommandPool = std::make_unique<GEVulkanCommandPool>();
	vkCommandPool->CreateCommandPool(vkLogicalDevice->getVkDevice(), vkPhysicalDevice->GetQueueFamilyIndices().graphicsFamily.value());

	frameContext->CreateCmdBuffer(vkCommandPool->GetCommandPool(), vkLogicalDevice->getVkDevice());
}

void VulkanRenderer::Cleanup()
{
    if(vkCommandPool)
    {
		vkCommandPool->Cleanup(vkLogicalDevice->getVkDevice());
        vkCommandPool.reset();
	}

    if(vkPipeline)
    {
		vkPipeline->Cleanup();
		vkPipeline.reset();
	}

    if(frameContext)
    {
		frameContext.reset();
	}

    if(vkRenderPass)
    {
        vkRenderPass->Cleanup();
		vkRenderPass.reset();
    }

	if (vkSwapChain)
	{
		vkSwapChain->Cleanup();
		vkSwapChain.reset();
    }
    if(vkSurfaceView)
    {
		vkSurfaceView->Cleanup();
        vkSurfaceView.reset();
	}

    if(vkPhysicalDevice)
    {
		vkPhysicalDevice->Cleanup();
		vkPhysicalDevice.reset();
	}

    if(vkLogicalDevice)
    {
		vkLogicalDevice->cleanUp();
		vkLogicalDevice.reset();
	}

    if (vkValidationLayer.IsValidationLayerEnabled()) {
       vkValidationLayer.DestroyDebugUtilsMessengerEXT(vkInstance, nullptr);
    }

    if(vkInstance)
    {
        vkDestroyInstance(vkInstance, nullptr);
    }
}

void VulkanRenderer::BeginFrame()
{
}

void VulkanRenderer::EndFrame()
{
}

void VulkanRenderer::SwapBuffers()
{
}

void VulkanRenderer::PrintInstanceExtensions()
{
    std::vector<VkExtensionProperties> instanceExtensions;
	uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported\n";
    instanceExtensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());

    for(const VkExtensionProperties &e : instanceExtensions)
    {
        std::cout << e.extensionName << std::endl;
    }

    std::cout << std::endl << std::endl;
}

void VulkanRenderer::SetRequiredExtensions(GERequiredExtensions* pRequiredExtensions) 
{ 
    requiredExtensions = *pRequiredExtensions; 
}

