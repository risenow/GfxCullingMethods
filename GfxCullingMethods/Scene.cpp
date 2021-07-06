#include "Scene.h"

Scene::Scene() {}

RenderStatistics Scene::Render(GraphicsDevice& device, Camera& camera, bool usePrevVisibility )
{
    RenderStatistics stats;
    if (usePrevVisibility)
    {
        for (std::vector<SuperMeshInstance*>& lastVisibleMeshes : m_LastVisibleMeshesLists)
            for (SuperMeshInstance* inst : lastVisibleMeshes)
                stats += inst->Render(device, camera);
        return stats;
    }

    m_LastVisibleMeshesLists.clear();
    m_LastVisibleMeshesLists.resize(m_Rooms.size());

    Camera::Frustum fr = Camera::Frustum(camera);
    m_CurrentRoom->GatherVisibleObjects(device, camera, fr, nullptr, m_LastVisibleMeshesLists);

    for (std::vector<SuperMeshInstance*>& lastVisibleMeshes : m_LastVisibleMeshesLists)
        for (SuperMeshInstance* inst : lastVisibleMeshes)
            stats += inst->Render(device, camera);

    return stats;
}

void Scene::Update(Camera& cam)
{
    m_CurrentRoom = m_CurrentRoom->RoomTransition(m_PrevCam, cam.GetPosition());
    m_PrevCam = cam.GetPosition();
}

void Scene::ConsumeRooms(const std::vector<PortalSystem::Room*>& rooms)
{
    for (PortalSystem::Room* room : m_Rooms)
        delete room;
    m_Rooms = rooms;
    if (rooms.size())
        m_CurrentRoom = rooms[0];
}

void Scene::ReleaseGPUData()
{
    for (PortalSystem::Room* room : m_Rooms)
        room->ReleaseGPUData();
}