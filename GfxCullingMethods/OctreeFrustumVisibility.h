#pragma once

#include "Visibility.h"
#include "Octree.h"
#include <unordered_set>
#include <assert.h>

class OctreeFrustumVisibility : public Visibility
{
public:
    OctreeFrustumVisibility(const std::vector<SuperMeshInstance*>& meshes) : m_LastUpdatedFrameIndex(1)
    {
        for (SuperMeshInstance* mesh : meshes)
        {
            m_Octree.Add(m_AABB, mesh->GetAABB(), mesh);
        }
    }

    virtual void Gather(GraphicsDevice& device, Camera& cam, std::vector<SuperMeshInstance*>& visibleMeshes) override final // to fix excess vector copy
    {
        if (m_LastUpdatedFrameIndex == device.GetFrameIndex())
        {
            visibleMeshes = m_LastVisibleMeshes;
            return;
        }
        m_LastUpdatedFrameIndex = device.GetFrameIndex();

        m_LastVisibleMeshes.clear();
        m_LastVisibleMeshes.reserve(m_Octree.GetManagedBVCount());

        Camera::Frustum frustum = Camera::Frustum(cam);

        m_Octree.GatherVisiblePayload(frustum, m_LastVisibleMeshes, 0, device.GetFrameIndex());

        visibleMeshes = m_LastVisibleMeshes;
    }
    void GatherFrustum(GraphicsDevice& device, const glm::vec3& camPos, Camera::Frustum& fr, std::vector<SuperMeshInstance*>& visibleMeshes) override final
    {
        if (m_LastUpdatedFrameIndex == device.GetFrameIndex())
        {
            visibleMeshes = m_LastVisibleMeshes;
            return;
        }
        m_LastUpdatedFrameIndex = device.GetFrameIndex();

        m_LastVisibleMeshes.clear();
        m_LastVisibleMeshes.reserve(m_Octree.GetManagedBVCount());

        Camera::Frustum frustum = fr;

        m_Octree.GatherVisiblePayload(frustum, m_LastVisibleMeshes, 0, device.GetFrameIndex());

        visibleMeshes = m_LastVisibleMeshes;
    }
    virtual void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild = false) override final
    {
        for (SuperMeshInstance* mesh : meshes)
            m_Octree.Add(m_AABB, mesh->GetAABB(), mesh);
    }
    virtual void AddMesh(SuperMeshInstance* mesh, bool rebuild = false) override final
    {
        m_Octree.Add(m_AABB, mesh->GetAABB(), mesh);
    }
    virtual void Rebuild() override final
    {
        m_Octree.Build();
    }
    virtual void Clear() override final
    {
        m_Octree.Clear();
        m_LastVisibleMeshes.resize(0);
    }
    virtual void SetSceneAABB(const AABB& aabb) override
    {
        m_AABB = aabb;
    }
    typedef PerfWideOctree<SuperMeshInstance*> MeshOctree;
    MeshOctree& GetOctree()// remove in future/final version
    {
        return m_Octree;
    }
private:
    MeshOctree m_Octree;
    std::vector<SuperMeshInstance*> m_LastVisibleMeshes;
    AABB m_AABB;
    uint64_t m_LastUpdatedFrameIndex;
};