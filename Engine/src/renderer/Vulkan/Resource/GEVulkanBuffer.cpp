#include <cstring>
#include <stdexcept>

#include "renderer/Vulkan/Resources/GEVulkanBuffer.h"
#include "renderer/Vulkan/Resources/GEVulkanStagingDevice.h"

namespace {

VkDeviceSize AlignDown(VkDeviceSize value, VkDeviceSize alignment)
{
	return value & ~(alignment - 1);
}

VkDeviceSize AlignUp(VkDeviceSize value, VkDeviceSize alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

VkDeviceSize GetNonCoherentAtomSize(VkPhysicalDevice physicalDevice)
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	return properties.limits.nonCoherentAtomSize;
}

} // namespace

uint32_t GEVulkanBuffer::FindMemoryType(
	VkPhysicalDevice physicalDevice,
	uint32_t typeFilter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if ((typeFilter & (1 << i)) &&
			(memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

GEVulkanBuffer::HostMemoryType GEVulkanBuffer::FindHostVisibleMemoryType(
	VkPhysicalDevice physicalDevice,
	uint32_t typeFilter)
{
	const VkMemoryPropertyFlags candidates[] = {
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
	};

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (VkMemoryPropertyFlags requiredFlags : candidates)
	{
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) &&
				(memProperties.memoryTypes[i].propertyFlags & requiredFlags) == requiredFlags)
			{
				return { i, memProperties.memoryTypes[i].propertyFlags };
			}
		}
	}

	throw std::runtime_error("failed to find host-visible memory type!");
}

bool GEVulkanBuffer::IsMemoryCoherent(VkMemoryPropertyFlags properties)
{
	return (properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
}

bool GEVulkanBuffer::IsHostVisible() const
{
	return (m_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
}

void GEVulkanBuffer::FlushMappedRange(
	VkDevice device,
	VkDeviceMemory memory,
	VkDeviceSize offset,
	VkDeviceSize size,
	VkDeviceSize nonCoherentAtomSize)
{
	const VkDeviceSize alignedOffset = AlignDown(offset, nonCoherentAtomSize);
	const VkDeviceSize alignedEnd = AlignUp(offset + size, nonCoherentAtomSize);

	VkMappedMemoryRange range{};
	range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range.memory = memory;
	range.offset = alignedOffset;
	range.size = alignedEnd - alignedOffset;

	if (vkFlushMappedMemoryRanges(device, 1, &range) != VK_SUCCESS)
		throw std::runtime_error("failed to flush mapped memory range!");
}

void GEVulkanBuffer::InvalidateMappedRange(
	VkDevice device,
	VkDeviceMemory memory,
	VkDeviceSize offset,
	VkDeviceSize size,
	VkDeviceSize nonCoherentAtomSize)
{
	const VkDeviceSize alignedOffset = AlignDown(offset, nonCoherentAtomSize);
	const VkDeviceSize alignedEnd = AlignUp(offset + size, nonCoherentAtomSize);

	VkMappedMemoryRange range{};
	range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	range.memory = memory;
	range.offset = alignedOffset;
	range.size = alignedEnd - alignedOffset;

	if (vkInvalidateMappedMemoryRanges(device, 1, &range) != VK_SUCCESS)
		throw std::runtime_error("failed to invalidate mapped memory range!");
}

void GEVulkanBuffer::BufferSubData(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkDeviceSize offset,
	VkDeviceSize size,
	const void* data)
{
	if (!m_mappedPtr)
		throw std::runtime_error("buffer is not persistently mapped!");

	if (offset + size > m_size)
		throw std::runtime_error("buffer subdata out of range!");

	std::memcpy(static_cast<uint8_t*>(m_mappedPtr) + offset, data, static_cast<size_t>(size));

	if (!IsMemoryCoherent(m_memoryProperties))
	{
		const VkDeviceSize atomSize = GetNonCoherentAtomSize(physicalDevice);
		FlushMappedRange(device, m_memory, offset, size, atomSize);
	}
}

GEVulkanSubmitResult GEVulkanBuffer::CopyBuffer(
	VkDevice device,
	VkCommandPool commandPool,
	VkQueue graphicsQueue,
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	VkDeviceSize srcOffset,
	VkDeviceSize dstOffset,
	VkDeviceSize size,
	VkBufferUsageFlags dstUsage)
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffer for buffer copy!");

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin command buffer for buffer copy!");

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = srcOffset;
	copyRegion.dstOffset = dstOffset;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	VkAccessFlags dstAccess = 0;

	if (dstUsage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
	{
		dstStage |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		dstAccess |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	}
	if (dstUsage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
	{
		dstStage |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		dstAccess |= VK_ACCESS_INDEX_READ_BIT;
	}
	if (dstUsage & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)
	{
		dstStage |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		dstAccess |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	}
	if (dstAccess == 0)
		dstAccess = VK_ACCESS_MEMORY_READ_BIT;

	VkBufferMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = dstAccess;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = dstBuffer;
	barrier.offset = dstOffset;
	barrier.size = size;

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		dstStage,
		0,
		0, nullptr,
		1, &barrier,
		0, nullptr);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to end command buffer for buffer copy!");

	VkFence fence;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS)
		throw std::runtime_error("failed to create fence for buffer copy!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, fence) != VK_SUCCESS)
	{
		vkDestroyFence(device, fence, nullptr);
		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		throw std::runtime_error("failed to submit buffer copy command buffer!");
	}

	return { fence, commandBuffer };
}

void GEVulkanBuffer::Create(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkDeviceSize size,
	VkBufferUsageFlags usage,
	VkMemoryPropertyFlags memoryProperties)
{
	Cleanup(device);

	m_size = size;
	m_usage = usage;
	m_memoryProperties = memoryProperties;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits,
		memoryProperties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate buffer memory!");

	vkBindBufferMemory(device, m_buffer, m_memory, 0);
}

void GEVulkanBuffer::CreateHostVisible(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkDeviceSize size,
	VkBufferUsageFlags usage)
{
	Cleanup(device);

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create host-visible buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_buffer, &memRequirements);

	const HostMemoryType hostMem = FindHostVisibleMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits);

	m_size = size;
	m_usage = usage;
	m_memoryProperties = hostMem.properties;

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = hostMem.typeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate host-visible buffer memory!");

	vkBindBufferMemory(device, m_buffer, m_memory, 0);

	if (vkMapMemory(device, m_memory, 0, size, 0, &m_mappedPtr) != VK_SUCCESS)
		throw std::runtime_error("failed to map host-visible buffer memory!");
}

void GEVulkanBuffer::CreateDeviceLocalFromData(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	GEVulkanStagingDevice& staging,
	const void* data,
	VkDeviceSize size,
	VkBufferUsageFlags usage)
{
	Create(
		physicalDevice,
		device,
		size,
		usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	staging.BufferSubData(*this, 0, static_cast<size_t>(size), data);
}

void GEVulkanBuffer::Cleanup(VkDevice device)
{
	if (m_mappedPtr != nullptr)
	{
		vkUnmapMemory(device, m_memory);
		m_mappedPtr = nullptr;
	}

	if (m_buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, m_buffer, nullptr);
		m_buffer = VK_NULL_HANDLE;
	}

	if (m_memory != VK_NULL_HANDLE)
	{
		vkFreeMemory(device, m_memory, nullptr);
		m_memory = VK_NULL_HANDLE;
	}

	m_size = 0;
	m_usage = 0;
	m_memoryProperties = 0;
}
