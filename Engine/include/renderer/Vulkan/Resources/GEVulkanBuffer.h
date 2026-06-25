#pragma once

#include <vulkan/vulkan.h>

class GEVulkanStagingDevice;

struct GEVulkanSubmitResult {
	VkFence fence = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
};

class GEVulkanBuffer {
public:
	GEVulkanBuffer() = default;
	~GEVulkanBuffer() = default;

	void Create(VkPhysicalDevice physicalDevice, VkDevice device,
		VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);

	void CreateHostVisible(VkPhysicalDevice physicalDevice, VkDevice device,
		VkDeviceSize size, VkBufferUsageFlags usage);

	void CreateDeviceLocalFromData(VkPhysicalDevice physicalDevice, VkDevice device,
		GEVulkanStagingDevice& staging,
		const void* data, VkDeviceSize size, VkBufferUsageFlags usage);

	void BufferSubData(VkPhysicalDevice physicalDevice, VkDevice device,
		VkDeviceSize offset, VkDeviceSize size, const void* data);

	void Cleanup(VkDevice device);

	VkBuffer GetBuffer() const { return m_buffer; }
	VkDeviceSize GetSize() const { return m_size; }
	VkBufferUsageFlags GetUsage() const { return m_usage; }
	bool IsValid() const { return m_buffer != VK_NULL_HANDLE; }
	bool IsHostVisible() const;
	bool IsMapped() const { return m_mappedPtr != nullptr; }
	void* GetMappedPtr() const { return m_mappedPtr; }
	VkMemoryPropertyFlags GetMemoryProperties() const { return m_memoryProperties; }

	static GEVulkanSubmitResult CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue,
		VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset,
		VkDeviceSize size, VkBufferUsageFlags dstUsage);

private:
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkDeviceSize m_size = 0;
	VkBufferUsageFlags m_usage = 0;
	VkMemoryPropertyFlags m_memoryProperties = 0;
	void* m_mappedPtr = nullptr;

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
};
