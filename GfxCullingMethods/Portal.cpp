#include "Portal.h"

void GenPortalFrustum(Camera::Frustum& prevFr, Camera::Frustum& fr, const glm::vec3& camPos, const AABB& portal)
{
    int maxXIndex = -1; float maxXScore = -FLT_MAX;
    int maxYIndex = -1; float maxYScore = -FLT_MAX;
    int minXIndex = -1; float minXScore = FLT_MAX;
    int minYIndex = -1; float minYScore = FLT_MAX;
    
    for (size_t i = 0; i < 8; i++)
    {
        glm::vec3 p = portal.GetPoint(i);
        float xScore = prevFr.planes[0].Test(p);
        float yScore = prevFr.planes[2].Test(p);

        if (xScore > maxXScore)
        {
            maxXIndex = i;
            maxXScore = xScore;
        }
        if (xScore < minXScore)
        {
            minXIndex = i;
            minXScore = xScore;
        }
        if (yScore > maxYScore)
        {
            maxYIndex = i;
            maxYScore = yScore;
        }
        if (yScore < minYScore)
        {
            minYIndex = i;
            minYScore = yScore;
        }
    }
    glm::vec3 minXP = portal.GetPoint(minXIndex);
    glm::vec3 maxXP = portal.GetPoint(maxXIndex);
    glm::vec3 minYP = portal.GetPoint(minYIndex);
    glm::vec3 maxYP = portal.GetPoint(maxYIndex);

    glm::vec3 rightVec = glm::normalize(prevFr.planes[0].n);
    glm::vec3 topVec = glm::normalize(prevFr.planes[2].n);
    glm::vec3 n;

    //left plane
    n = glm::cross(minXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glm::dot(n, rightVec))
        fr.planes[0].n = n;
    else
        fr.planes[0].n = -n;
    fr.planes[0].d = -glm::dot(fr.planes[0].n, minXP);

    //right plane
    n = glm::cross(maxXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    if (glm::dot(n, rightVec))
        fr.planes[1].n = -n;
    else
        fr.planes[1].n = n;
    fr.planes[1].d = -glm::dot(fr.planes[1].n, maxXP);

    //bottom plane
    n = glm::cross(minYP - camPos, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glm::dot(n, topVec))
        fr.planes[2].n = n;
    else
        fr.planes[2].n = -n;
    fr.planes[2].d = -glm::dot(fr.planes[2].n, minYP);

    //top plane
    n = glm::cross(maxYP - camPos, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glm::dot(n, topVec))
        fr.planes[3].n = -n;
    else
        fr.planes[3].n = n;
    fr.planes[3].d = -glm::dot(fr.planes[3].n, maxYP);

    fr.planes[4] = prevFr.planes[4];
    fr.planes[5] = prevFr.planes[5];
}

Portal::Portal()
{
    for (Room*& r : m_Rooms)
        r = nullptr;
}
bool Portal::Valid()
{
    return m_Rooms[0] && m_Rooms[1];
}
AABB Portal::GetAABB()
{
    return m_AABB;
}
void Portal::GatherVisibleObject(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<SuperMeshInstance*>& meshInstances)
{
    Camera::Frustum cutFrustum = Camera::Frustum(cam);
    GenPortalFrustum(cutFrustum, cutFrustum, cam.GetPosition(), m_AABB);

    for (Room* room : m_Rooms)
        if (room != from)
            room->GatherVisibleObjects(device, cam, cutFrustum, this, meshInstances);
}

void Room::GatherVisibleObjects(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<SuperMeshInstance*>& meshInstances)
{
    m_BVHVis.Gather(device, cam, meshInstances);

    for (Portal* portal : m_Portals)
        if (portal != from && actualFrustum.Test(portal->GetAABB()))
            portal->GatherVisibleObject(device, cam, actualFrustum, this, meshInstances);
}