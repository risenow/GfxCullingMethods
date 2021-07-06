#include "BVHFrustumVisibility.h"

BVHFrustumVisibility::BVHFrustumVisibility(const std::vector<SuperMeshInstance*>& meshes) : m_LastUpdatedFrameIndex(1)
{
    for (SuperMeshInstance* mesh : meshes)
    {
        m_BVH.Add(mesh->GetAABB(), mesh);
    }
}

void BVHFrustumVisibility::Gather(GraphicsDevice& device, Camera& cam, std::vector<SuperMeshInstance*>& visibleMeshes)  // to fix excess vector copy
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
void BVHFrustumVisibility::GatherFrustum(GraphicsDevice& device, const glm::vec3& camPos, Camera::Frustum& fr, std::vector<SuperMeshInstance*>& visibleMeshes)
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

    MicrosecondsTimer timer;
    timer.Begin();
    m_BVH.GatherVisiblePayload(frustum, m_LastVisibleMeshes);
    __int64 dur = timer.End();
    std::cout << "gath dur: " + std::to_string(dur) << std::endl;

    visibleMeshes = m_LastVisibleMeshes;
}
void BVHFrustumVisibility::AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild) 
{
    for (SuperMeshInstance* mesh : meshes)
    {
        m_BVH.Add(mesh->GetAABB(), mesh);
        m_CAABB.Extend(mesh->GetAABB());
    }

    if (rebuild)
        Rebuild();
}
void BVHFrustumVisibility::AddMesh(SuperMeshInstance* mesh, bool rebuild) 
{
    m_BVH.Add(mesh->GetAABB(), mesh);
    m_CAABB.Extend(mesh->GetAABB().m_Centroid);

    glm::vec3 centroid = mesh->GetAABB().m_Centroid;

    if (rebuild)
        Rebuild();
}
void BVHFrustumVisibility::Rebuild()
{
    m_BVH.Build();
}
void BVHFrustumVisibility::Clear()
{
    m_BVH.Clear();
    m_LastVisibleMeshes.resize(0);
}

BVHFrustumVisibility::MeshBVH& BVHFrustumVisibility::GetBVH() // remove in future/final version
{
    return m_BVH;
}

void BVHFrustumVisibility::ReleaseGPUData()
{
    std::vector<SuperMeshInstance*> meshes;
    m_BVH.GatherAllPayload(meshes);

    for (SuperMeshInstance* mesh : meshes)
        delete mesh;
}
void BVHFrustumVisibility::GatherAll(std::vector<SuperMeshInstance*>& meshes)
{
    m_BVH.GatherAllPayload(meshes);
}