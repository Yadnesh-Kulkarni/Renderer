#include <iostream>
#include <vector>
#include <array>
#include <cstdint>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "renderer/Vulkan/core/GEVulkan.h"
#include "utils/FileUtils.h"
#include "GLFW/glfw3.h"

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

void VulkanRenderer::RecordCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = vkRenderPass->GetRenderPass();
	renderPassInfo.framebuffer = vkSwapChain->GetFramebuffer(imageIndex);
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = vkSwapChain->GetSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdSetViewport(commandBuffer, 0, 1, &frameContext->GetViewport());
	vkCmdSetScissor(commandBuffer, 0, 1, &frameContext->GetScissor());

	VkExtent2D extent = vkSwapChain->GetSwapChainExtent();
	float aspect = static_cast<float>(extent.width) / static_cast<float>(extent.height);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(m_rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 view  = glm::lookAt(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 proj  = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
	proj[1][1] *= -1.0f;

	glm::mat4 mvp = proj * view * model;

	struct CubePushConstants {
		glm::mat4 mvp;
		int32_t isWireframe;
	};

	constexpr uint32_t pushConstantSize = static_cast<uint32_t>(sizeof(glm::mat4) + sizeof(int32_t));
	const VkShaderStageFlags pushConstantStages = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

	CubePushConstants pushConstants{};
	pushConstants.mvp = mvp;

	VkBuffer vertexBuffers[] = { m_vertexBuffer.GetBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, m_indexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

	// Draw filled mesh
	pushConstants.isWireframe = 0;
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline->GetGraphicsPipeline());
	vkCmdPushConstants(commandBuffer, vkPipeline->GetPipelineLayout(),
		pushConstantStages, 0, pushConstantSize, &pushConstants);
	vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);

	// Draw wireframe overlay
	pushConstants.isWireframe = 1;
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipelineWireframe->GetGraphicsPipeline());
	vkCmdPushConstants(commandBuffer, vkPipelineWireframe->GetPipelineLayout(),
		pushConstantStages, 0, pushConstantSize, &pushConstants);
	vkCmdDrawIndexed(commandBuffer, m_indexCount, 1, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}

	m_rotationAngle += 0.5f; // Rotate 0.5 degrees per frame
}

void VulkanRenderer::CreateRenderFinishedSemaphores(uint32_t imageCount)
{
	renderFinishedSemaphores.resize(imageCount);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (uint32_t i = 0; i < imageCount; i++)
	{
		if (vkCreateSemaphore(vkLogicalDevice->getVkDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render finished semaphores!");
		}
	}
}

void VulkanRenderer::DestroyRenderFinishedSemaphores()
{
	for (VkSemaphore& semaphore : renderFinishedSemaphores)
	{
		if (semaphore != VK_NULL_HANDLE)
		{
			vkDestroySemaphore(vkLogicalDevice->getVkDevice(), semaphore, nullptr);
			semaphore = VK_NULL_HANDLE;
		}
	}
	renderFinishedSemaphores.clear();
}

void VulkanRenderer::RecreateSwapChain()
{
	GEFramebufferSize fbSize = m_window->getFramebufferSize();
	while (fbSize.width == 0 || fbSize.height == 0)
	{
		glfwWaitEvents();
		fbSize = m_window->getFramebufferSize();
	}

	VkExtent2D current = vkSwapChain->GetSwapChainExtent();
	if (fbSize.width == static_cast<int>(current.width) &&
		fbSize.height == static_cast<int>(current.height))
	{
		m_window->ClearResizeFlag();
		return;
	}

	WaitIdle();

	DestroyRenderFinishedSemaphores();
	vkSwapChain->Cleanup();

	vkPhysicalDevice->RefreshSwapchainSupportDetails();
	vkSwapChain->SetDimensions(fbSize.width, fbSize.height);
	vkSwapChain->CreateSwapChain();
	vkSwapChain->CreateFramebuffers(m_depthFormat);

	imagesInFlight.assign(vkSwapChain->GetImageCount(), VK_NULL_HANDLE);
	CreateRenderFinishedSemaphores(vkSwapChain->GetImageCount());

	frameContext->UpdateExtent(
		static_cast<uint32_t>(fbSize.width),
		static_cast<uint32_t>(fbSize.height));
}

VulkanRenderer::VulkanRenderer(WindowCreator* window) : Renderer(window)
{
	
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::Initialize()
{
	SetRequiredExtensions();

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

	m_depthFormat = vkPhysicalDevice->ResolveDepthBufferFormat();
	std::cout << "Depth buffer format: "
	          << (m_depthFormat == DEPTH_BUFFER_PREFERRED_FORMAT ? "D32_SFLOAT (preferred)" : "hardware fallback")
	          << "\n";

	vkRenderPass = std::make_unique<GEVulkanRenderPass>(
		vkLogicalDevice->getVkDevice(),
		vkSwapChain->GetSwapChainImageFormat(),
		m_depthFormat);
	vkRenderPass->CreateRenderPass();

	frameContext = std::make_unique<GEVulkanFrameContext>(vkLogicalDevice->getVkDevice(),fbSize.width, fbSize.height);

	vkPipeline = std::make_unique<GEVulkanPipeline>(vkLogicalDevice->getVkDevice(), *frameContext, *vkRenderPass);
	vkPipeline->CreatePipeline("App/Shaders/shader.vert", "App/Shaders/shader.frag", VK_POLYGON_MODE_FILL);

	vkPipelineWireframe = std::make_unique<GEVulkanPipeline>(vkLogicalDevice->getVkDevice(), *frameContext, *vkRenderPass);
	vkPipelineWireframe->CreatePipeline("App/Shaders/shader.vert", "App/Shaders/shader.frag", VK_POLYGON_MODE_LINE);

	vkSwapChain->SetRenderPass(vkRenderPass.get());
	vkSwapChain->CreateFramebuffers(m_depthFormat);

	imagesInFlight.resize(vkSwapChain->GetImageCount(), VK_NULL_HANDLE);
	CreateRenderFinishedSemaphores(vkSwapChain->GetImageCount());

	vkCommandPool = std::make_unique<GEVulkanCommandPool>();
	vkCommandPool->CreateCommandPool(vkLogicalDevice->getVkDevice(), vkPhysicalDevice->GetQueueFamilyIndices().graphicsFamily.value());

	frameContext->CreateCmdBuffers(vkCommandPool->GetCommandPool());

	const std::string modelPath = FileUtils::ResolvePath("Models/rubber_duck/scene.gltf");
	if (!m_meshLoader.LoadFromFile(modelPath))
		throw std::runtime_error("Failed to load model: " + m_meshLoader.GetLastError());

	std::cout << "Loaded rubber duck: " << m_meshLoader.GetVertices().size()
	          << " vertices, " << m_meshLoader.GetIndices().size() << " indices\n";

	const auto& vertices = m_meshLoader.GetVertices();
	const auto& indices = m_meshLoader.GetIndices();

	m_vertexBuffer.CreateDeviceLocalFromData(
		vkPhysicalDevice->GetPhysicalDevice(),
		vkLogicalDevice->getVkDevice(),
		vkCommandPool->GetCommandPool(),
		vkLogicalDevice->getGraphicsQueue(),
		vertices.data(),
		vertices.size() * sizeof(MeshVertex),
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

	m_indexBuffer.CreateDeviceLocalFromData(
		vkPhysicalDevice->GetPhysicalDevice(),
		vkLogicalDevice->getVkDevice(),
		vkCommandPool->GetCommandPool(),
		vkLogicalDevice->getGraphicsQueue(),
		indices.data(),
		indices.size() * sizeof(uint32_t),
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

	m_indexCount = static_cast<uint32_t>(indices.size());
}

void VulkanRenderer::Cleanup()
{
	WaitIdle();

	DestroyRenderFinishedSemaphores();

	if (vkLogicalDevice)
	{
		m_vertexBuffer.Cleanup(vkLogicalDevice->getVkDevice());
		m_indexBuffer.Cleanup(vkLogicalDevice->getVkDevice());
	}

    if(vkCommandPool)
    {
		vkCommandPool->Cleanup(vkLogicalDevice->getVkDevice());
        vkCommandPool.reset();
	}

    if(vkPipelineWireframe)
    {
		vkPipelineWireframe->Cleanup();
		vkPipelineWireframe.reset();
	}

    if(vkPipeline)
    {
		vkPipeline->Cleanup();
		vkPipeline.reset();
	}

    if(frameContext)
    {
		frameContext->Cleanup();
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
	frameContext->WaitForCurrentFrameFence();

	VkResult acquireResult = vkAcquireNextImageKHR(
		vkLogicalDevice->getVkDevice(),
		vkSwapChain->GetVkSwapChain(),
		UINT64_MAX,
		frameContext->GetImageAvailableSemaphore(),
		VK_NULL_HANDLE,
		&currentImageIndex);

	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		frameReady = false;
		return;
	}
	if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	if (imagesInFlight[currentImageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(vkLogicalDevice->getVkDevice(), 1, &imagesInFlight[currentImageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[currentImageIndex] = frameContext->GetInFlightFence();

	frameContext->ResetCurrentFrameFence();
	vkResetCommandBuffer(frameContext->GetCommandBuffer(), 0);
	frameReady = true;
}

void VulkanRenderer::EndFrame()
{
	if (!frameReady)
	{
		return;
	}

	VkCommandBuffer commandBuffer = frameContext->GetCommandBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { frameContext->GetImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT
	};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphore = renderFinishedSemaphores[currentImageIndex];
	VkSemaphore signalSemaphores[] = { signalSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vkLogicalDevice->getGraphicsQueue(), 1, &submitInfo, frameContext->GetInFlightFence()) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { vkSwapChain->GetVkSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &currentImageIndex;
	presentInfo.pResults = nullptr;

	VkResult presentResult = vkQueuePresentKHR(vkLogicalDevice->getPresentQueue(), &presentInfo);

	if (m_window->WasResized() ||
		presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
		presentResult == VK_SUBOPTIMAL_KHR)
	{
		RecreateSwapChain();
	}
	else if (presentResult != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	frameContext->AdvanceFrame();
}

void VulkanRenderer::DrawFrame()
{
	if (!frameReady)
	{
		return;
	}

	RecordCommands(frameContext->GetCommandBuffer(), currentImageIndex);
}

void VulkanRenderer::WaitIdle()
{
	vkDeviceWaitIdle(vkLogicalDevice->getVkDevice());
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

void VulkanRenderer::SetRequiredExtensions() 
{ 
    m_window->getRequiredExtensions(&requiredExtensions);
}

