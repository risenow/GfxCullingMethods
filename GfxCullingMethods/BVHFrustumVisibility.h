#pragma once

#include "Visibility.h"
#include "MicrosecondsTimer.h"
#include "BVH.h"

class BVHFrustumVisibility : public Visibility
{
public:
    BVHFrustumVisibility(const std::vector<SuperMeshInstance*>& meshes);

    virtual void Gather(GraphicsDevice& device, Camera& cam, std::vector<SuperMeshInstance*>& visibleMeshes) override final; // to fix excess vector copy
    void GatherFrustum(GraphicsDevice& device, const glm::vec3& camPos, Camera::Frustum& fr, std::vector<SuperMeshInstance*>& visibleMeshes) override final;
    virtual void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild = false) override final;
    virtual void AddMesh(SuperMeshInstance* mesh, bool rebuild = false) override final;
    virtual void Rebuild();
    virtual void Clear();
    
    typedef BVH<SuperMeshInstance*> MeshBVH;

    MeshBVH& GetBVH();

    void ReleaseGPUData();
private:
    void GatherAll(std::vector<SuperMeshInstance*>& meshes);

    MeshBVH m_BVH;
    std::vector<SuperMeshInstance*> m_LastVisibleMeshes;
    AABB m_CAABB;
    uint64_t m_LastUpdatedFrameIndex;
};