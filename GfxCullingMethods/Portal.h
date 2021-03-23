#pragma once
#include <vector>
#include "AABB.h"
#include "SuperMeshInstance.h"
#include "BVHFrustumVisibility.h"
#include "Camera.h"

class Room;
class Portal
{
public:
    void GatherVisibleObject(Camera::Frustum& fr, std::vector<SuperMeshInstance*>& meshInstances);
private:
    std::vector<Room*> m_Rooms;
    AABB m_AABB;
};

class Room
{
public:
    void GatherVisibleObjects(Camera::Frustum& fr, std::vector<SuperMeshInstance*>& meshInstances);
private:
    std::vector<Portal*> m_Portals;
    BVHFrustumVisibility m_BVHVis;
};