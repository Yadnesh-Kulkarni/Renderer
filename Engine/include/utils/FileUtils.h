#pragma once
#include <string>

namespace FileUtils {
    // Returns path joined to the current base directory.
    // Absolute paths are returned unchanged.
    std::string ResolvePath(const std::string& relativePath);

    // Override the base directory at runtime (defaults to ENGINE_SOURCE_DIR).
    void SetBaseDirectory(const std::string& dir);

    const std::string& GetBaseDirectory();
}
