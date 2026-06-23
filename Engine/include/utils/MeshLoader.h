#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct MeshVertex {
    float position[3];
    float normal[3];
    float texCoord[2];
};

class MeshLoader {
public:
    bool LoadFromFile(const std::string& path);
    void Clear();

    const std::vector<MeshVertex>& GetVertices() const { return m_vertices; }
    const std::vector<uint32_t>& GetIndices() const { return m_indices; }
    const std::string& GetLastError() const { return m_lastError; }
    bool IsLoaded() const { return !m_vertices.empty() && !m_indices.empty(); }

private:
    std::vector<MeshVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::string m_lastError;
};
