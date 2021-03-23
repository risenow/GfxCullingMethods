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
    Portal();
    bool Valid();
    void GatherVisibleObject(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<SuperMeshInstance*>& meshInstances);

    AABB GetAABB();
private:
    Room* m_Rooms[2];
    AABB m_AABB;
};

class Room
{
public:
    void GatherVisibleObjects(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<SuperMeshInstance*>& meshInstances);
private:
    std::vector<Portal*> m_Portals;
    BVHFrustumVisibility m_BVHVis;
};