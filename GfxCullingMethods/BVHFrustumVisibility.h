#pragma once

#include "Visibility.h"
#include "BVH.h"

class BVHFrustumVisibility : public Visibility
{
public:
    BVHFrustumVisibility(const std::vector<SuperMeshInstance*>& meshes) : m_LastUpdatedFrameIndex(1)
    {
        for (SuperMeshInstance* mesh : meshes)
        {
            m_BVH.Add(mesh->GetAABB(), mesh);
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
        m_LastVisibleMeshes.reserve(m_BVH.GetBVCount());

        Camera::Frustum frustum = Camera::Frustum(cam);

        m_BVH.GatherVisiblePayload(frustum, m_LastVisibleMeshes);

        visibleMeshes = m_LastVisibleMeshes;
    }
    void GatherFrustum(GraphicsDevice& device, Camera::Frustum& fr, std::vector<SuperMeshInstance*>& visibleMeshes)
    {
        if (m_LastUpdatedFrameIndex == device.GetFrameIndex())
        {
            visibleMeshes = m_LastVisibleMeshes;
            return;
        }
        m_LastUpdatedFrameIndex = device.GetFrameIndex();

        m_LastVisibleMeshes.clear();
        m_LastVisibleMeshes.reserve(m_BVH.GetBVCount());

        Camera::Frustum frustum = fr;

        m_BVH.GatherVisiblePayload(frustum, m_LastVisibleMeshes);

        visibleMeshes = m_LastVisibleMeshes;
    }
    virtual void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild = false) override final
    {
        for (SuperMeshInstance* mesh : meshes)
        {
            m_BVH.Add( mesh->GetAABB(), mesh );
            m_CAABB.Extend(mesh->GetAABB());
        }

        if (rebuild)
            Rebuild();
    }
    virtual void AddMesh(SuperMeshInstance* mesh, bool rebuild = false) override final
    {
        m_BVH.Add(mesh->GetAABB(), mesh);
        m_CAABB.Extend(mesh->GetAABB().m_Centroid);

        glm::vec3 centroid = mesh->GetAABB().m_Centroid;

        if (rebuild)
            Rebuild();
    }
    virtual void Rebuild() override final 
    {
        m_BVH.Build();
    }
    virtual void Clear() override final
    {
        m_BVH.Clear();
        m_LastVisibleMeshes.resize(0);
    }
    
    typedef BVH<SuperMeshInstance*> MeshBVH;

    MeshBVH& GetBVH() // remove in future/final version
    {
        return m_BVH;
    } 
private:
    MeshBVH m_BVH;
    std::vector<SuperMeshInstance*> m_LastVisibleMeshes;
    AABB m_CAABB;
    uint64_t m_LastUpdatedFrameIndex;
};