#pragma once
#include "Visibility.h"

class LinearFrustumVisibility : public Visibility
{
public:
    LinearFrustumVisibility(const std::vector<SuperMeshInstance*>& meshes) : m_Meshes(meshes), m_LastUpdatedFrameIndex(1)
    {}

    virtual void Gather(GraphicsDevice& device, Camera& cam, std::vector<SuperMeshInstance*>& visibleMeshes) override final // to fix excess vector copy
    {
        if (m_LastUpdatedFrameIndex == device.GetFrameIndex())
        {
            visibleMeshes = m_LastVisibleMeshes;
            return;
        }
        m_LastUpdatedFrameIndex = device.GetFrameIndex();

        m_LastVisibleMeshes.clear();
        m_LastVisibleMeshes.reserve(m_Meshes.size());

        Camera::Frustum frustum = Camera::Frustum(cam);

        for (SuperMeshInstance* mesh : m_Meshes)
        {
            if (frustum.Test(mesh->GetAABB()))
                m_LastVisibleMeshes.push_back(mesh);
        }

        visibleMeshes = m_LastVisibleMeshes;
    }
    virtual void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild = false) override final
    {
        m_Meshes.reserve(m_Meshes.size() + meshes.size());
        m_Meshes.insert(m_Meshes.end(), meshes.begin(), meshes.end());
    }
    virtual void AddMesh(SuperMeshInstance* mesh, bool rebuild = false) override final
    {
        m_Meshes.push_back(mesh);
    }
    virtual void Rebuild() override final {}
    virtual void Clear() override final
    {
        m_Meshes.resize(0);
        m_LastVisibleMeshes.resize(0);
    }
private:
    uint64_t m_LastUpdatedFrameIndex;
    std::vector<SuperMeshInstance*> m_Meshes;
    std::vector<SuperMeshInstance*> m_LastVisibleMeshes;
};