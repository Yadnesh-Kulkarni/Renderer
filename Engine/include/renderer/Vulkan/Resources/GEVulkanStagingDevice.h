#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "renderer/Vulkan/Resources/GEVulkanBuffer.h"
#include "renderer/Vulkan/Resources/GEVulkanImage.h"

class GEVulkanStagingDevice {
public:
	GEVulkanStagingDevice() = default;
	~GEVulkanStagingDevice() = default;

	GEVulkanStagingDevice(const GEVulkanStagingDevice&) = delete;
	GEVulkanStagingDevice& operator=(const GEVulkanStagingDevice&) = delete;

	void Initialize(VkPhysicalDevice physicalDevice, VkDevice device,
		VkCommandPool commandPool, VkQueue graphicsQueue,
		VkDeviceSize maxStagingSize = 0);

	void Cleanup();

	void BufferSubData(GEVulkanBuffer& dst, size_t dstOffset, size_t size, const void* data);
	void ImageData2D(GEVulkanImage& image, const VkRect2D& region,
		uint32_t mipLevel, VkFormat format, const void* data,
		uint32_t bufferRowLength = 0);

	void WaitIdle();

private:
	static constexpr uint32_t kStagingBufferAlignment = 16;
	static constexpr VkDeviceSize kMinBufferSize = 16u * 1024u * 1024u;
	static constexpr VkDeviceSize kMaxBufferSizeCap = 256u * 1024u * 1024u;

	struct MemoryRegionDesc {
		uint64_t offset = 0;
		uint64_t size = 0;
		VkFence fence = VK_NULL_HANDLE;
		VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	};

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device = VK_NULL_HANDLE;
	VkCommandPool m_commandPool = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue = VK_NULL_HANDLE;
	VkDeviceSize m_maxBufferSize = 0;

	GEVulkanBuffer m_stagingBuffer;
	VkDeviceSize m_stagingBufferSize = 0;
	std::vector<MemoryRegionDesc> m_regions;

	MemoryRegionDesc GetNextFreeOffset(uint32_t size);
	void EnsureStagingBufferSize(uint32_t sizeNeeded);
	void WaitAndReset();
	bool IsRegionReady(const MemoryRegionDesc& region) const;
	void ReleaseSubmit(VkFence fence, VkCommandBuffer commandBuffer);

	GEVulkanSubmitResult SubmitCopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer,
		VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size,
		VkBufferUsageFlags dstUsage);

	GEVulkanSubmitResult SubmitImageUpload(VkBuffer srcBuffer, VkDeviceSize srcOffset,
		GEVulkanImage& image, const VkRect2D& region, uint32_t mipLevel,
		VkImageLayout oldLayout);

	static uint32_t GetAlignedSize(uint32_t size, uint32_t alignment);
	static uint32_t GetBytesPerPixel(VkFormat format);
	static uint32_t GetTextureBytesPerLayer(uint32_t width, uint32_t height, VkFormat format, uint32_t mipLevel);
};
