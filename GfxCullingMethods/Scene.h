#pragma once
#include "GraphicsDevice.h"
#include "Camera.h"
#include "Portal.h"
#include <fstream>

class Scene
{
public:
    Scene() {}

    RenderStatistics Render(GraphicsDevice& device, Camera& camera, bool usePrevVisibility = false)
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

    void Update(Camera& cam)
    {
        m_CurrentRoom = m_CurrentRoom->RoomTransition(m_PrevCam, cam.GetPosition());
        m_PrevCam = cam.GetPosition();
    }

    void ConsumeRooms(const std::vector<PortalSystem::Room*>& rooms) 
    {
        for (PortalSystem::Room* room : m_Rooms)
            delete room;
        m_Rooms = rooms;
        if (rooms.size())
            m_CurrentRoom = rooms[0];
    }

    void ReleaseGPUData()
    {
        for (PortalSystem::Room* room : m_Rooms)
            room->ReleaseGPUData();
    }
private:
    glm::vec3 m_PrevCam;
    PortalSystem::Room* m_CurrentRoom;
    std::vector<PortalSystem::Room*> m_Rooms;

    std::vector<std::vector<SuperMeshInstance*>> m_LastVisibleMeshesLists;
};