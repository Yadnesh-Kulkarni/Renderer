#include <cstring>
#include <stdexcept>

#include "renderer/Vulkan/Resources/GEVulkanBuffer.h"

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

void GEVulkanBuffer::WriteToMappedMemory(
	VkDevice device,
	VkPhysicalDevice physicalDevice,
	VkDeviceMemory memory,
	VkDeviceSize offset,
	const void* data,
	VkDeviceSize size,
	VkMemoryPropertyFlags properties)
{
	void* mapped = nullptr;
	if (vkMapMemory(device, memory, offset, size, 0, &mapped) != VK_SUCCESS)
		throw std::runtime_error("failed to map buffer memory!");

	std::memcpy(mapped, data, static_cast<size_t>(size));

	if (!IsMemoryCoherent(properties))
	{
		const VkDeviceSize atomSize = GetNonCoherentAtomSize(physicalDevice);
		FlushMappedRange(device, memory, offset, size, atomSize);
	}

	vkUnmapMemory(device, memory);
}

void GEVulkanBuffer::CopyBuffer(
	VkDevice device,
	VkCommandPool commandPool,
	VkQueue graphicsQueue,
	VkBuffer srcBuffer,
	VkBuffer dstBuffer,
	VkDeviceSize size)
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
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to end command buffer for buffer copy!");

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		throw std::runtime_error("failed to submit buffer copy command buffer!");

	vkQueueWaitIdle(graphicsQueue);
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
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

void GEVulkanBuffer::CreateDeviceLocalFromData(
	VkPhysicalDevice physicalDevice,
	VkDevice device,
	VkCommandPool commandPool,
	VkQueue graphicsQueue,
	const void* data,
	VkDeviceSize size,
	VkBufferUsageFlags usage)
{
	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create staging buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

	const HostMemoryType hostMem = FindHostVisibleMemoryType(
		physicalDevice,
		memRequirements.memoryTypeBits);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = hostMem.typeIndex;

	if (vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate staging buffer memory!");

	vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);

	WriteToMappedMemory(device, physicalDevice, stagingMemory, 0, data, size, hostMem.properties);

	Create(
		physicalDevice,
		device,
		size,
		usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, commandPool, graphicsQueue, stagingBuffer, m_buffer, size);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingMemory, nullptr);
}

void GEVulkanBuffer::Cleanup(VkDevice device)
{
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
	m_memoryProperties = 0;
}
