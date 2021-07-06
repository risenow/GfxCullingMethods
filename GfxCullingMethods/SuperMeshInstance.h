#pragma once
#include "SuperMesh.h"

class SuperMeshInstance
{
public:
    SuperMeshInstance(SuperMesh* mesh, const glm::mat4x4& modelMatrix);

    float GetScaleFactor();
    glm::vec3 GetScaleFactor3();
    RenderStatistics Render(GraphicsDevice& device, Camera& camera);
    AABB GetAABB();
    AABB GetSubAABB(size_t i);

    void SetTransform(const glm::mat4x4& transform);
    glm::mat4x4 GetTransform() const;
    SuperMesh* GetSuperMesh() const;
    

private:
    SuperMesh* m_Mesh;
    glm::mat4x4 m_Transform;
};