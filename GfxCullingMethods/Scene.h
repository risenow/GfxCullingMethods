#pragma once
#include "GraphicsDevice.h"
#include "Camera.h"
#include "Portal.h"
#include <fstream>

class Scene
{
public:
    Scene();

    RenderStatistics Render(GraphicsDevice& device, Camera& camera, bool usePrevVisibility = false);

    void Update(Camera& cam);
    void ConsumeRooms(const std::vector<PortalSystem::Room*>& rooms);

    void ReleaseGPUData();
private:
    glm::vec3 m_PrevCam;
    PortalSystem::Room* m_CurrentRoom;
    std::vector<PortalSystem::Room*> m_Rooms;

    std::vector<std::vector<SuperMeshInstance*>> m_LastVisibleMeshesLists;
};