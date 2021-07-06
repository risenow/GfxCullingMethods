#pragma once
#include "Mesh.h"

class SuperMesh
{
public:
    SuperMesh();
    SuperMesh(const std::vector<Mesh*> meshes);

    RenderStatistics Render(GraphicsDevice& device, Camera& camera, const glm::mat4x4& modelMatrix = glm::identity<glm::mat4x4>());

    static void FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file, SuperMesh& mesh);
    static SuperMesh* FromFile(GraphicsDevice& device, GraphicsTextureCollection& textureCollection, const std::string& file);

    Mesh* GetSubMesh(size_t i);
    size_t GetSubMeshesCount();

    void ReleaseGPUData();

    AABB GetAABB() const;
private:
    AABB m_AABB;
    std::vector<Mesh*> m_Meshes;
};