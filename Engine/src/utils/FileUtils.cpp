#include "utils/FileUtils.h"
#include <filesystem>

namespace FileUtils {

namespace {
    std::string s_baseDir =
#ifdef ENGINE_SOURCE_DIR
        ENGINE_SOURCE_DIR;
#else
        "";
#endif
}

void SetBaseDirectory(const std::string& dir)
{
    s_baseDir = dir;
    if (!s_baseDir.empty() && s_baseDir.back() != '/' && s_baseDir.back() != '\\')
        s_baseDir += '/';
}

const std::string& GetBaseDirectory()
{
    return s_baseDir;
}

std::string ResolvePath(const std::string& relativePath)
{
    std::filesystem::path p(relativePath);
    if (p.is_absolute())
        return relativePath;
    return (std::filesystem::path(s_baseDir) / p).make_preferred().string();
}

} // namespace FileUtils
