#pragma once

#include <cstdint>
#include <string>
#include <vector>

class ImageLoader {
public:
	bool LoadFromFile(const std::string& path);
	void Clear();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetChannelCount() const { return m_channels; }
	const uint8_t* GetPixels() const { return m_pixels.data(); }

private:
	int m_width = 0;
	int m_height = 0;
	int m_channels = 0;
	std::vector<uint8_t> m_pixels;
};
