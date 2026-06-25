#include <stdexcept>

#include "renderer/Vulkan/Resources/GEVulkanTexture.h"
#include "utils/ImageLoader.h"

VkDescriptorSetLayout GEVulkanTexture::CreateDescriptorSetLayout(VkDevice device)
{
	VkDescriptorSetLayoutBinding samplerBinding{};
	samplerBinding.binding = 0;
	samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerBinding.descriptorCount = 1;
	samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &samplerBinding;

	VkDescriptorSetLayout layout = VK_NULL_HANDLE;
	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture descriptor set layout!");

	return layout;
}

void GEVulkanTexture::DestroyDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout)
{
	if (layout != VK_NULL_HANDLE)
		vkDestroyDescriptorSetLayout(device, layout, nullptr);
}

void GEVulkanTexture::LoadFromFile(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	GEVulkanStagingDevice& staging,
	VkDescriptorSetLayout descriptorSetLayout,
	const std::string& path)
{
	Cleanup(device);

	ImageLoader imageLoader;
	if (!imageLoader.LoadFromFile(path))
		throw std::runtime_error("failed to load texture: " + path);

	const uint32_t width = static_cast<uint32_t>(imageLoader.GetWidth());
	const uint32_t height = static_cast<uint32_t>(imageLoader.GetHeight());
	const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

	m_image.Create(physicalDevice, device, width, height, format);

	const VkRect2D region{
		{ 0, 0 },
		{ width, height },
	};
	staging.ImageData2D(m_image, region, 0, format, imageLoader.GetPixels());
	staging.WaitIdle();

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1.0f;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture descriptor pool!");

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSet) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate texture descriptor set!");

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = m_image.GetImageView();
	imageInfo.sampler = m_sampler;

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = m_descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
}

void GEVulkanTexture::Cleanup(VkDevice device)
{
	if (m_descriptorPool != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);
		m_descriptorPool = VK_NULL_HANDLE;
	}

	m_descriptorSet = VK_NULL_HANDLE;

	if (m_sampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(device, m_sampler, nullptr);
		m_sampler = VK_NULL_HANDLE;
	}

	m_image.Cleanup(device);
}
