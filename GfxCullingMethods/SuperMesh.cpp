#include "SuperMesh.h"

SuperMesh::SuperMesh() {}
SuperMesh::SuperMesh(const std::vector<Mesh*> meshes) : m_Meshes(meshes)
{
    for (Mesh* mesh : meshes)
        m_AABB.Extend(mesh->GetAABB());
}

RenderStatistics SuperMesh::Render(GraphicsDevice& device, Camera& camera, const glm::mat4x4& modelMatrix)
{
    RenderStatistics stats;
    for (Mesh* mesh : m_Meshes)
        stats += mesh->Render(device, camera, modelMatrix);
    return stats;
}

void SuperMesh::FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file, SuperMesh& mesh)
{
    Mesh::LoadFromFile(device, textureCollection, file, mesh.m_Meshes, mesh.m_AABB);
}
SuperMesh* SuperMesh::FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file)
{
    SuperMesh* mesh = new SuperMesh();
    FromFile(device, textureCollection, file, *mesh);

    return mesh;
}

Mesh* SuperMesh::GetSubMesh(size_t i)
{
    return m_Meshes[i];
}
size_t SuperMesh::GetSubMeshesCount()
{
    return m_Meshes.size();
}

void SuperMesh::ReleaseGPUData()
{
}

AABB SuperMesh::GetAABB() const
{
    return m_AABB;
}