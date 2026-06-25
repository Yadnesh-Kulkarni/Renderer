#include <algorithm>
#include <cstring>
#include <stdexcept>

#include "renderer/Vulkan/Resources/GEVulkanStagingDevice.h"

void GEVulkanStagingDevice::Initialize(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkCommandPool commandPool,
	VkQueue graphicsQueue,
	VkDeviceSize maxStagingSize)
{
	m_physicalDevice = physicalDevice;
	m_device = device;
	m_commandPool = commandPool;
	m_graphicsQueue = graphicsQueue;

	if (maxStagingSize != 0)
	{
		m_maxBufferSize = maxStagingSize;
	}
	else
	{
		VkPhysicalDeviceVulkan11Properties props11{};
		props11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
		VkPhysicalDeviceProperties2 props2{};
		props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		props2.pNext = &props11;
		vkGetPhysicalDeviceProperties2(physicalDevice, &props2);
		m_maxBufferSize = props11.maxMemoryAllocationSize;
	}

	m_maxBufferSize = std::min(m_maxBufferSize, kMaxBufferSizeCap);
	m_maxBufferSize = std::max(m_maxBufferSize, kMinBufferSize);
}

void GEVulkanStagingDevice::Cleanup()
{
	WaitIdle();

	if (m_stagingBuffer.IsValid())
		m_stagingBuffer.Cleanup(m_device);

	m_stagingBufferSize = 0;
	m_regions.clear();
}

bool GEVulkanStagingDevice::IsRegionReady(const MemoryRegionDesc& region) const
{
	if (region.fence == VK_NULL_HANDLE)
		return true;

	return vkGetFenceStatus(m_device, region.fence) == VK_SUCCESS;
}

void GEVulkanStagingDevice::ReleaseSubmit(VkFence fence, VkCommandBuffer commandBuffer)
{
	if (fence != VK_NULL_HANDLE)
	{
		if (vkWaitForFences(m_device, 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS)
			throw std::runtime_error("failed to wait for staging fence!");

		vkDestroyFence(m_device, fence, nullptr);
	}

	if (commandBuffer != VK_NULL_HANDLE)
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
}

void GEVulkanStagingDevice::WaitAndReset()
{
	for (const MemoryRegionDesc& region : m_regions)
		ReleaseSubmit(region.fence, region.commandBuffer);

	m_regions.clear();
	if (m_stagingBufferSize > 0)
		m_regions.push_back({ 0, m_stagingBufferSize, VK_NULL_HANDLE, VK_NULL_HANDLE });
}

void GEVulkanStagingDevice::WaitIdle()
{
	WaitAndReset();
}

uint32_t GEVulkanStagingDevice::GetAlignedSize(uint32_t size, uint32_t alignment)
{
	return (size + alignment - 1) & ~(alignment - 1);
}

uint32_t GEVulkanStagingDevice::GetBytesPerPixel(VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_R8_UNORM:
	case VK_FORMAT_R8_SRGB:
		return 1;
	case VK_FORMAT_R8G8_UNORM:
	case VK_FORMAT_R8G8_SRGB:
	case VK_FORMAT_R16_UNORM:
	case VK_FORMAT_R16_SFLOAT:
		return 2;
	case VK_FORMAT_R8G8B8_UNORM:
	case VK_FORMAT_R8G8B8_SRGB:
		return 3;
	case VK_FORMAT_R8G8B8A8_UNORM:
	case VK_FORMAT_R8G8B8A8_SRGB:
	case VK_FORMAT_B8G8R8A8_UNORM:
	case VK_FORMAT_B8G8R8A8_SRGB:
	case VK_FORMAT_R16G16_UNORM:
	case VK_FORMAT_R16G16_SFLOAT:
	case VK_FORMAT_R32_SFLOAT:
	case VK_FORMAT_R32_UINT:
	case VK_FORMAT_R32_SINT:
		return 4;
	case VK_FORMAT_R16G16B16A16_UNORM:
	case VK_FORMAT_R16G16B16A16_SFLOAT:
	case VK_FORMAT_R32G32_SFLOAT:
		return 8;
	case VK_FORMAT_R32G32B32A32_SFLOAT:
		return 16;
	default:
		throw std::runtime_error("unsupported format for staging image upload!");
	}
}

uint32_t GEVulkanStagingDevice::GetTextureBytesPerLayer(
	uint32_t width, uint32_t height, VkFormat format, uint32_t mipLevel)
{
	const uint32_t mipWidth = std::max(1u, width >> mipLevel);
	const uint32_t mipHeight = std::max(1u, height >> mipLevel);
	return mipWidth * mipHeight * GetBytesPerPixel(format);
}

void GEVulkanStagingDevice::EnsureStagingBufferSize(uint32_t sizeNeeded)
{
	const uint32_t alignedSize = GetAlignedSize(
		std::max(sizeNeeded, static_cast<uint32_t>(kMinBufferSize)),
		kStagingBufferAlignment);

	const uint32_t clampedSize = static_cast<uint32_t>(
		std::min(static_cast<VkDeviceSize>(alignedSize), m_maxBufferSize));

	if (m_stagingBuffer.IsValid())
	{
		if (clampedSize <= m_stagingBufferSize || m_stagingBufferSize == m_maxBufferSize)
			return;
	}

	WaitAndReset();

	if (m_stagingBuffer.IsValid())
		m_stagingBuffer.Cleanup(m_device);

	m_stagingBufferSize = clampedSize;
	m_stagingBuffer.CreateHostVisible(
		m_physicalDevice,
		m_device,
		m_stagingBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	m_regions.clear();
	m_regions.push_back({ 0, m_stagingBufferSize, VK_NULL_HANDLE, VK_NULL_HANDLE });
}

GEVulkanStagingDevice::MemoryRegionDesc GEVulkanStagingDevice::GetNextFreeOffset(uint32_t size)
{
	const uint32_t requestedSize = GetAlignedSize(size, kStagingBufferAlignment);

	EnsureStagingBufferSize(requestedSize);

	if (m_regions.empty())
		m_regions.push_back({ 0, m_stagingBufferSize, VK_NULL_HANDLE, VK_NULL_HANDLE });

	auto bestIt = m_regions.begin();

	for (auto it = m_regions.begin(); it != m_regions.end(); ++it)
	{
		if (!IsRegionReady(*it))
			continue;

		if (it->size >= requestedSize)
		{
			const uint64_t unusedSize = it->size - requestedSize;
			const uint64_t unusedOffset = it->offset + requestedSize;
			const uint64_t usedOffset = it->offset;

			if (it->fence != VK_NULL_HANDLE || it->commandBuffer != VK_NULL_HANDLE)
				ReleaseSubmit(it->fence, it->commandBuffer);

			it = m_regions.erase(it);
			if (unusedSize > 0)
				m_regions.insert(m_regions.begin(), { unusedOffset, unusedSize, VK_NULL_HANDLE, VK_NULL_HANDLE });

			return { usedOffset, requestedSize, VK_NULL_HANDLE, VK_NULL_HANDLE };
		}

		if (it->size > bestIt->size)
			bestIt = it;
	}

	if (bestIt != m_regions.end() && IsRegionReady(*bestIt))
	{
		MemoryRegionDesc result = *bestIt;
		if (result.fence != VK_NULL_HANDLE || result.commandBuffer != VK_NULL_HANDLE)
			ReleaseSubmit(result.fence, result.commandBuffer);
		result.fence = VK_NULL_HANDLE;
		result.commandBuffer = VK_NULL_HANDLE;
		m_regions.erase(bestIt);
		return result;
	}

	WaitAndReset();

	const uint64_t unusedSize = m_stagingBufferSize > requestedSize
		? m_stagingBufferSize - requestedSize
		: 0;

	m_regions.clear();
	if (unusedSize > 0)
	{
		const uint64_t unusedOffset = m_stagingBufferSize - unusedSize;
		m_regions.push_back({ unusedOffset, unusedSize, VK_NULL_HANDLE, VK_NULL_HANDLE });
	}

	return {
		0,
		m_stagingBufferSize - unusedSize,
		VK_NULL_HANDLE,
		VK_NULL_HANDLE,
	};
}

GEVulkanSubmitResult GEVulkanStagingDevice::SubmitCopyBuffer(
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	VkDeviceSize srcOffset,
	VkDeviceSize dstOffset,
	VkDeviceSize size,
	VkBufferUsageFlags dstUsage)
{
	return GEVulkanBuffer::CopyBuffer(
		m_device,
		m_commandPool,
		m_graphicsQueue,
		srcBuffer,
		dstBuffer,
		srcOffset,
		dstOffset,
		size,
		dstUsage);
}

GEVulkanSubmitResult GEVulkanStagingDevice::SubmitImageUpload(
	VkBuffer srcBuffer,
	VkDeviceSize srcOffset,
	GEVulkanImage& image,
	const VkRect2D& region,
	uint32_t mipLevel,
	VkImageLayout oldLayout)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer for image upload!");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin command buffer for image upload!");

	VkImageMemoryBarrier toTransfer{};
	toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	toTransfer.srcAccessMask = 0;
	toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	toTransfer.oldLayout = oldLayout;
	toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toTransfer.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	toTransfer.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	toTransfer.image = image.GetImage();
	toTransfer.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	toTransfer.subresourceRange.baseMipLevel = mipLevel;
	toTransfer.subresourceRange.levelCount = 1;
	toTransfer.subresourceRange.baseArrayLayer = 0;
	toTransfer.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &toTransfer);

	VkBufferImageCopy copyRegion{};
	copyRegion.bufferOffset = srcOffset;
	copyRegion.bufferRowLength = 0;
	copyRegion.bufferImageHeight = 0;
	copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.imageSubresource.mipLevel = mipLevel;
	copyRegion.imageSubresource.baseArrayLayer = 0;
	copyRegion.imageSubresource.layerCount = 1;
	copyRegion.imageOffset = { static_cast<int32_t>(region.offset.x), static_cast<int32_t>(region.offset.y), 0 };
	copyRegion.imageExtent = { region.extent.width, region.extent.height, 1 };

	vkCmdCopyBufferToImage(
		commandBuffer,
		srcBuffer,
		image.GetImage(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copyRegion);

	VkImageMemoryBarrier toShaderRead{};
	toShaderRead.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	toShaderRead.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	toShaderRead.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	toShaderRead.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	toShaderRead.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	toShaderRead.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	toShaderRead.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	toShaderRead.image = image.GetImage();
	toShaderRead.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	toShaderRead.subresourceRange.baseMipLevel = mipLevel;
	toShaderRead.subresourceRange.levelCount = 1;
	toShaderRead.subresourceRange.baseArrayLayer = 0;
	toShaderRead.subresourceRange.layerCount = 1;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0,
		0, nullptr,
		0, nullptr,
		1, &toShaderRead);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to end command buffer for image upload!");

	VkFence fence;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (vkCreateFence(m_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
		throw std::runtime_error("failed to create fence for image upload!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
	{
		vkDestroyFence(m_device, fence, nullptr);
		vkFreeCommandBuffers(m_device, m_commandPool, 1, &commandBuffer);
		throw std::runtime_error("failed to submit image upload command buffer!");
	}

	return { fence, commandBuffer };
}

void GEVulkanStagingDevice::BufferSubData(
	GEVulkanBuffer& dst,
	size_t dstOffset,
	size_t size,
	const void* data)
{
	if (!data || size == 0)
		return;

	if (dstOffset + size > dst.GetSize())
		throw std::runtime_error("staging buffer subdata out of range!");

	if (dst.IsHostVisible())
	{
		dst.BufferSubData(m_physicalDevice, m_device, dstOffset, size, data);
		return;
	}

	size_t remaining = size;
	size_t currentDstOffset = dstOffset;
	const uint8_t* srcData = static_cast<const uint8_t*>(data);

	while (remaining > 0)
	{
		MemoryRegionDesc desc = GetNextFreeOffset(static_cast<uint32_t>(remaining));
		const size_t chunkSize = std::min(remaining, static_cast<size_t>(desc.size));

		m_stagingBuffer.BufferSubData(
			m_physicalDevice,
			m_device,
			desc.offset,
			chunkSize,
			srcData);

		const GEVulkanSubmitResult submit = SubmitCopyBuffer(
			m_stagingBuffer.GetBuffer(),
			dst.GetBuffer(),
			desc.offset,
			currentDstOffset,
			chunkSize,
			dst.GetUsage());

		desc.fence = submit.fence;
		desc.commandBuffer = submit.commandBuffer;
		m_regions.push_back(desc);

		remaining -= chunkSize;
		srcData += chunkSize;
		currentDstOffset += chunkSize;
	}
}

void GEVulkanStagingDevice::ImageData2D(
	GEVulkanImage& image,
	const VkRect2D& region,
	uint32_t mipLevel,
	VkFormat format,
	const void* data,
	uint32_t bufferRowLength)
{
	if (!data)
		return;

	if (bufferRowLength != 0)
		throw std::runtime_error("bufferRowLength is not supported yet!");

	const uint32_t storageSize = GetTextureBytesPerLayer(
		region.extent.width,
		region.extent.height,
		format,
		0);

	EnsureStagingBufferSize(storageSize);

	MemoryRegionDesc desc = GetNextFreeOffset(storageSize);
	if (desc.size < storageSize)
	{
		WaitAndReset();
		desc = GetNextFreeOffset(storageSize);
	}

	if (desc.size < storageSize)
		throw std::runtime_error("staging buffer too small for image upload!");

	m_stagingBuffer.BufferSubData(
		m_physicalDevice,
		m_device,
		desc.offset,
		storageSize,
		data);

	const VkImageLayout oldLayout = image.GetLayout();
	const bool coversFullImage = region.offset.x == 0 && region.offset.y == 0 &&
		region.extent.width == image.GetExtent().width &&
		region.extent.height == image.GetExtent().height;

	const VkImageLayout srcLayout = coversFullImage && oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
		? VK_IMAGE_LAYOUT_UNDEFINED
		: oldLayout;

	const GEVulkanSubmitResult submit = SubmitImageUpload(
		m_stagingBuffer.GetBuffer(),
		desc.offset,
		image,
		region,
		mipLevel,
		srcLayout);

	desc.fence = submit.fence;
	desc.commandBuffer = submit.commandBuffer;
	m_regions.push_back(desc);
	image.SetLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}
