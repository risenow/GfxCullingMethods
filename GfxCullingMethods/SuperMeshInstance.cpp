#include "SuperMeshInstance.h"

SuperMeshInstance::SuperMeshInstance(SuperMesh* mesh, const glm::mat4x4& modelMatrix) : m_Mesh(mesh), m_Transform(modelMatrix)
{}

float SuperMeshInstance::GetScaleFactor()
{
    return glm::length(glm::vec3(m_Transform[0]));
}
glm::vec3 SuperMeshInstance::GetScaleFactor3()
{
    return glm::vec3(glm::length(glm::vec3(m_Transform[0])), glm::length(glm::vec3(m_Transform[1])), glm::length(glm::vec3(m_Transform[2])));
}
RenderStatistics SuperMeshInstance::Render(GraphicsDevice& device, Camera& camera)
{
    return m_Mesh->Render(device, camera, m_Transform);
}
AABB SuperMeshInstance::GetAABB()
{
    AABB aabb = m_Mesh->GetAABB();
    return AABB(glm::vec3(m_Transform * glm::vec4(aabb.m_Min, 1.0)), glm::vec3(m_Transform * glm::vec4(aabb.m_Max, 1.0)));
}
AABB SuperMeshInstance::GetSubAABB(size_t i)
{
    AABB aabb = m_Mesh->GetSubMesh(i)->GetAABB();
    return AABB(glm::vec3(m_Transform * glm::vec4(aabb.m_Min, 1.0)), glm::vec3(m_Transform * glm::vec4(aabb.m_Max, 1.0)));
}

void SuperMeshInstance::SetTransform(const glm::mat4x4& transform)
{
    m_Transform = transform;
}
glm::mat4x4 SuperMeshInstance::GetTransform() const
{
    return m_Transform;
}
SuperMesh* SuperMeshInstance::GetSuperMesh() const
{
    return m_Mesh;
}