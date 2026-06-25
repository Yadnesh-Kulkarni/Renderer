#include "utils/ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

bool ImageLoader::LoadFromFile(const std::string& path)
{
	Clear();

	int width = 0;
	int height = 0;
	int channels = 0;
	uint8_t* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
	{
		std::cerr << "ImageLoader: failed to load '" << path << "': " << stbi_failure_reason() << "\n";
		return false;
	}

	m_width = width;
	m_height = height;
	m_channels = 4;
	m_pixels.assign(pixels, pixels + static_cast<size_t>(width) * static_cast<size_t>(height) * 4);
	stbi_image_free(pixels);
	return true;
}

void ImageLoader::Clear()
{
	m_width = 0;
	m_height = 0;
	m_channels = 0;
	m_pixels.clear();
}
