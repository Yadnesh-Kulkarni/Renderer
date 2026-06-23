#include "utils/MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace {

MeshVertex ToVertex(const aiMesh& mesh, unsigned i)
{
    MeshVertex vertex{};

    const aiVector3D& pos = mesh.mVertices[i];
    vertex.position[0] = pos.x;
    vertex.position[1] = pos.y;
    vertex.position[2] = pos.z;

    if (mesh.HasNormals())
    {
        const aiVector3D& normal = mesh.mNormals[i];
        vertex.normal[0] = normal.x;
        vertex.normal[1] = normal.y;
        vertex.normal[2] = normal.z;
    }

    if (mesh.HasTextureCoords(0))
    {
        const aiVector3D& uv = mesh.mTextureCoords[0][i];
        vertex.texCoord[0] = uv.x;
        vertex.texCoord[1] = uv.y;
    }

    return vertex;
}

} // namespace

void MeshLoader::Clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_lastError.clear();
}

bool MeshLoader::LoadFromFile(const std::string& path)
{
    Clear();

    Assimp::Importer importer;
    constexpr unsigned int importFlags =
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_JoinIdenticalVertices |
        aiProcess_FlipUVs |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality;

    const aiScene* scene = importer.ReadFile(path, importFlags);
    if (!scene)
    {
        m_lastError = importer.GetErrorString();
        return false;
    }

    if ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || !scene->mRootNode)
    {
        m_lastError = "Incomplete scene data";
        return false;
    }

    m_vertices.reserve(scene->mNumMeshes * 3);
    m_indices.reserve(scene->mNumMeshes * 3);

    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        const aiMesh* mesh = scene->mMeshes[meshIndex];
        if (!mesh)
            continue;

        const uint32_t vertexOffset = static_cast<uint32_t>(m_vertices.size());

        m_vertices.reserve(m_vertices.size() + mesh->mNumVertices);
        for (unsigned i = 0; i < mesh->mNumVertices; ++i)
            m_vertices.push_back(ToVertex(*mesh, i));

        for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
        {
            const aiFace& face = mesh->mFaces[faceIndex];
            if (face.mNumIndices != 3)
                continue;

            for (unsigned j = 0; j < 3; ++j)
                m_indices.push_back(vertexOffset + face.mIndices[j]);
        }
    }

    if (m_vertices.empty() || m_indices.empty())
    {
        m_vertices.clear();
        m_indices.clear();
        m_lastError = "No geometry found in model";
        return false;
    }

    return true;
}
