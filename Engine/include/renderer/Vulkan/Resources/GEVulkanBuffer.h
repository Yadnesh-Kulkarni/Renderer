#pragma once

#include <vulkan/vulkan.h>

class GEVulkanBuffer {
public:
	GEVulkanBuffer() = default;
	~GEVulkanBuffer() = default;

	void Create(VkPhysicalDevice physicalDevice, VkDevice device,
		VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);

	void CreateDeviceLocalFromData(VkPhysicalDevice physicalDevice, VkDevice device,
		VkCommandPool commandPool, VkQueue graphicsQueue,
		const void* data, VkDeviceSize size, VkBufferUsageFlags usage);

	void Cleanup(VkDevice device);

	VkBuffer GetBuffer() const { return m_buffer; }
	VkDeviceSize GetSize() const { return m_size; }
	bool IsValid() const { return m_buffer != VK_NULL_HANDLE; }
	VkMemoryPropertyFlags GetMemoryProperties() const { return m_memoryProperties; }

private:
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkDeviceSize m_size = 0;
	VkMemoryPropertyFlags m_memoryProperties = 0;

	struct HostMemoryType {
		uint32_t typeIndex = 0;
		VkMemoryPropertyFlags properties = 0;
	};

	static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice,
		uint32_t typeFilter, VkMemoryPropertyFlags properties);

	static HostMemoryType FindHostVisibleMemoryType(VkPhysicalDevice physicalDevice,
		uint32_t typeFilter);

	static bool IsMemoryCoherent(VkMemoryPropertyFlags properties);

	static void FlushMappedRange(VkDevice device, VkDeviceMemory memory,
		VkDeviceSize offset, VkDeviceSize size, VkDeviceSize nonCoherentAtomSize);

	static void InvalidateMappedRange(VkDevice device, VkDeviceMemory memory,
		VkDeviceSize offset, VkDeviceSize size, VkDeviceSize nonCoherentAtomSize);

	static void WriteToMappedMemory(VkDevice device, VkPhysicalDevice physicalDevice,
		VkDeviceMemory memory, VkDeviceSize offset, const void* data, VkDeviceSize size,
		VkMemoryPropertyFlags properties);

	static void CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue,
		VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
