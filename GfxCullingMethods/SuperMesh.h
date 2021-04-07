#pragma once
#include "Mesh.h"

class SuperMesh
{
public:
    SuperMesh() {}
    SuperMesh(const std::vector<Mesh*> meshes) : m_Meshes(meshes) 
    {
        for (Mesh* mesh : meshes)
            m_AABB.Extend(mesh->GetAABB());
    }

    RenderStatistics Render(GraphicsDevice& device, Camera& camera, const glm::mat4x4& modelMatrix = glm::identity<glm::mat4x4>())
    {
        RenderStatistics stats;
        for (Mesh* mesh : m_Meshes)
            stats += mesh->Render(device, camera, modelMatrix);
        return stats;
    }

    static void FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file, SuperMesh& mesh)
    {
        Mesh::LoadFromFile(device, textureCollection, file, mesh.m_Meshes, mesh.m_AABB);
    }
    static SuperMesh* FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file)
    {
        SuperMesh* mesh = new SuperMesh();
        FromFile(device, textureCollection, file, *mesh);

        return mesh;
    }

    Mesh* GetSubMesh(size_t i)
    {
        return m_Meshes[i];
    }
    size_t GetSubMeshesCount()
    {
        return m_Meshes.size();
    }

    void ReleaseGPUData()
    {
    }

    AABB GetAABB() const
    {
        return m_AABB;
    }
private:
    AABB m_AABB;
    std::vector<Mesh*> m_Meshes;
};