#pragma once
#include <array>
#include <cstddef>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include "utils/MeshLoader.h"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapchainSupportDetails {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct GEPhysicalDeviceDetails {
	QueueFamilyIndices queueFamilyIndices;
	SwapchainSupportDetails swapchainSupportDetails;
};

// Format_Z_F32 — preferred depth format; resolved at runtime with hardware fallback.
constexpr VkFormat DEPTH_BUFFER_PREFERRED_FORMAT = VK_FORMAT_D32_SFLOAT;

inline void GetMeshVertexInputDescriptions(
	VkVertexInputBindingDescription& binding,
	std::array<VkVertexInputAttributeDescription, 3>& attributes)
{
	binding.binding = 0;
	binding.stride = sizeof(MeshVertex);
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	attributes[0] = { 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, position) };
	attributes[1] = { 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(MeshVertex, normal) };
	attributes[2] = { 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(MeshVertex, texCoord) };
}
