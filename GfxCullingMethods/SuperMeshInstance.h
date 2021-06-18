#pragma once
#include "SuperMesh.h"

class SuperMeshInstance
{
public:
    SuperMeshInstance(SuperMesh* mesh, const glm::mat4x4& modelMatrix) : m_Mesh(mesh), m_Transform(modelMatrix)
    {}

    float GetScaleFactor()
    {
        return glm::length(glm::vec3(m_Transform[0]));
    }
    glm::vec3 GetScaleFactor3()
    {
        return glm::vec3(glm::length(glm::vec3(m_Transform[0])), glm::length(glm::vec3(m_Transform[1])), glm::length(glm::vec3(m_Transform[2])));
    }
    RenderStatistics Render(GraphicsDevice& device, Camera& camera)
    {
        return m_Mesh->Render(device, camera, m_Transform);
    }
    AABB GetAABB()
    {
        AABB aabb = m_Mesh->GetAABB();
        return AABB(glm::vec3(m_Transform * glm::vec4(aabb.m_Min, 1.0)), glm::vec3(m_Transform * glm::vec4(aabb.m_Max, 1.0)));
    }

    void SetTransform(const glm::mat4x4& transform)
    {
        m_Transform = transform;
    }
    glm::mat4x4 GetTransform() const
    {
        return m_Transform;
    }
    SuperMesh* GetSuperMesh() const
    {
        return m_Mesh;
    }
    /*void ProduceInstancesPerSubMesh(std::vector<SuperMeshInstance*>& meshes)
    {
        meshes.resize(m_Mesh->GetSubMeshesCount())
        for (size_t i = 0; i < m_Mesh->GetSubMeshesCount(); i++)
            meshes.push_back(new SuperMeshInstance())
    }*/
    

private:
    SuperMesh* m_Mesh;
    glm::mat4x4 m_Transform;
};