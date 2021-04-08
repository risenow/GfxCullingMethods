#include "Portal.h"
#include "glm/gtx/norm.hpp"

using namespace PortalSystem;

void PortalSystem::GenPortalFrustum(Camera::Frustum& prevFr, Camera::Frustum& fr, const glm::vec3& camPos, const glm::vec3& rightV, const glm::vec3& topV, const AABB& portal)
{
    if (portal.Contains(camPos))
    {
        fr = prevFr;
        return;
    }
    int maxXIndex = -1; float maxXScore = -FLT_MAX;
    int maxYIndex = -1; float maxYScore = -FLT_MAX;
    int minXIndex = -1; float minXScore = FLT_MAX;
    int minYIndex = -1; float minYScore = FLT_MAX;
    
    glm::vec3 front = portal.m_Centroid - camPos;
    glm::vec3 right = -glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 top = glm::cross(front, right);

    for (size_t i = 0; i < 8; i++)
    {
        glm::vec3 p = portal.GetPoint(i);
        float xScore = glm::dot(p, right);
        float yScore = glm::dot(p, top);

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

    glm::vec3 rightVec = right;
    glm::vec3 topVec = top;
    glm::vec3 n;

    //left plane
    n = glm::cross(minXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    fr.planes[0].n = -n;
    fr.planes[0].d = -glm::dot(fr.planes[0].n, minXP);

    //right plane
    n = glm::cross(maxXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    fr.planes[1].n = n;
    fr.planes[1].d = -glm::dot(fr.planes[1].n, maxXP);

    //bottom plane
    n = glm::cross(minYP - camPos, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glm::dot(n, topVec) > 0.0f)
        fr.planes[3].n = n;
    else
        fr.planes[3].n = -n;
    fr.planes[3].d = -glm::dot(fr.planes[3].n, minYP);

    //top plane
    n = glm::cross(maxYP - camPos, glm::vec3(1.0f, 0.0f, 0.0f));
    if (glm::dot(n, topVec) > 0.0f)
        fr.planes[2].n = -n;
    else
        fr.planes[2].n = n;
    fr.planes[2].d = -glm::dot(fr.planes[2].n, maxYP);

    fr.planes[4] = prevFr.planes[4];
    fr.planes[5] = prevFr.planes[5];

    Plane middleQuasiXZPlane;
    n = glm::cross((maxYP + minYP)*0.5f - camPos, right);
    if (glm::dot(n, top) < 0.0f)
        n = -n;
    middleQuasiXZPlane.n = glm::normalize(top);

    glm::vec3 leftPlN_XZ = glm::normalize(ProjectVecOnPlane(fr.planes[0].n, middleQuasiXZPlane.n));
    glm::vec3 prevLeftPlN_XZ = glm::normalize(ProjectVecOnPlane(prevFr.planes[0].n, middleQuasiXZPlane.n));
    glm::vec3 rightPlN_XZ = glm::normalize(ProjectVecOnPlane(fr.planes[1].n, middleQuasiXZPlane.n));
    glm::vec3 prevRightPlN_XZ = glm::normalize(ProjectVecOnPlane(prevFr.planes[1].n, middleQuasiXZPlane.n));
    float maxAngle = acos(glm::dot(leftPlN_XZ, rightPlN_XZ));
    glm::vec3 c = -glm::cross(prevLeftPlN_XZ, leftPlN_XZ);
    if (glm::dot(c, top) > 0.0f || glm::length2(prevLeftPlN_XZ) < 0.0001) 
    {
        fr.planes[0] = prevFr.planes[0];
    }
    else
    {
        float angle = acos(glm::dot(leftPlN_XZ, prevLeftPlN_XZ));
        if (angle > maxAngle)
        {
            fr.planes[0] = prevFr.planes[0];
        }
    }
    c = glm::cross(prevRightPlN_XZ, rightPlN_XZ);
    if (glm::dot(c, top) > 0.0f || glm::length2(prevRightPlN_XZ) < 0.0001) 
    {
        fr.planes[1] = prevFr.planes[1];
    }
    else
    {
        float angle = acos(glm::dot(rightPlN_XZ, prevRightPlN_XZ));
        if (angle > maxAngle)
        {
            fr.planes[1] = prevFr.planes[1];
        }
    }

    Plane middleQuasiYZPlane;
    middleQuasiYZPlane.n = glm::normalize(right);

    glm::vec3 topPlN_XZ = glm::normalize(ProjectVecOnPlane(fr.planes[2].n, middleQuasiYZPlane.n));
    glm::vec3 prevTopPlN_XZ = glm::normalize(ProjectVecOnPlane(prevFr.planes[2].n, middleQuasiYZPlane.n));
    glm::vec3 botPlN_XZ = glm::normalize(ProjectVecOnPlane(fr.planes[3].n, middleQuasiYZPlane.n));
    glm::vec3 prevBotPlN_XZ = glm::normalize(ProjectVecOnPlane(prevFr.planes[3].n, middleQuasiYZPlane.n));
    maxAngle = acos(glm::dot(topPlN_XZ, botPlN_XZ));
    c = glm::cross(prevBotPlN_XZ, botPlN_XZ);
    if (glm::dot(c, right) > 0.0f || glm::length2(prevBotPlN_XZ) < 0.0001) 
    {
        fr.planes[3] = prevFr.planes[3];
    }
    else
    {
        float angle = acos(glm::dot(botPlN_XZ, prevBotPlN_XZ));
        if (angle > maxAngle)
        {
            fr.planes[3] = prevFr.planes[3];
        }
    }
    c = -glm::cross(prevTopPlN_XZ, topPlN_XZ);
    if (glm::dot(c, right) > 0.0f || glm::length2(prevTopPlN_XZ) < 0.0001) 
    {
        fr.planes[2] = prevFr.planes[2];
    }
    else
    {
        float angle = acos(glm::dot(topPlN_XZ, prevTopPlN_XZ));
        if (angle > maxAngle)
        {
            fr.planes[2] = prevFr.planes[2];
        }
    }

}

Portal::Portal()
{
    for (Room*& r : m_Rooms)
        r = nullptr;
}
Portal::Portal(const LoPoApproxGeom& approxGeom, Room* room1, Room* room2)
{
    m_Rooms[0] = room1;
    m_Rooms[1] = room2;

    m_PortalGeom = approxGeom;
}
Room* Portal::GetTransition(Room* from)
{
    return m_Rooms[0] == from ? m_Rooms[1] : m_Rooms[0];
}
bool Portal::Valid()
{
    return m_Rooms[0] && m_Rooms[1];
}
AABB Portal::GetAABB()
{
    static const glm::vec3 epsOffset = glm::vec3(0.001, 0.001, 0.001);

    AABB aabb = m_PortalGeom.CalcAABB();
    aabb.m_Max += epsOffset;
    aabb.m_Min -= epsOffset;

    return aabb;
}
void Portal::GatherVisibleObjects(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<std::vector<SuperMeshInstance*>>& meshInstancesLists, size_t meshInstsListIndex)
{
    Camera::Frustum camFrust = Camera::Frustum(cam);
    Camera::Frustum cutFrustum = Camera::Frustum(cam);

    GenPortalFrustum(camFrust, cutFrustum, cam.GetPosition(), cam.GetRightVec(), cam.GetTopVec(), GetAABB());

    for (Room* room : m_Rooms)
        if (room != from)
            room->GatherVisibleObjects(device, cam, cutFrustum, this, meshInstancesLists, meshInstsListIndex);
}
bool Portal::ExactCheckSegIntersection(const glm::vec3& orig, const glm::vec3& dir)
{
    return m_PortalGeom.IntersectSeg(orig, dir);
}
Room::Room() : m_BVHVis({})
{}
void Room::GatherVisibleObjects(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<std::vector<SuperMeshInstance*>>& meshInstancesLists, size_t meshInstsListIndex)
{
    if (meshInstsListIndex >= meshInstancesLists.size())
        meshInstancesLists.resize(meshInstsListIndex + 1);

    std::vector<SuperMeshInstance*>& meshInstances = meshInstancesLists[meshInstsListIndex];

    m_BVHVis.GatherFrustum(device, cam.GetPosition(), actualFrustum, meshInstances);
    for (SuperMeshInstance* mesh : m_Meshes)
        meshInstances.push_back(mesh);

    for (DirectedPortal& p : m_Portals)
        if (p.portal != from && actualFrustum.Test(p.portal->GetAABB()))
            p.portal->GatherVisibleObjects(device, cam, actualFrustum, this, meshInstancesLists, meshInstsListIndex + 1);
}
void Room::AddMesh(SuperMeshInstance* mesh, bool applyVis, bool rebuildsVis)
{
    if (applyVis)
        m_BVHVis.AddMesh(mesh, rebuildsVis);
    else
    {
        m_Meshes.push_back(mesh);
        if (rebuildsVis)
            m_BVHVis.Rebuild();
    }
}

void Room::AddPortal(Portal* portal, const glm::vec3& dir)
{
    m_Portals.push_back({ portal, dir });
}
Room* Room::RoomTransition(const glm::vec3& prevCamPos, const glm::vec3& camPos)
{
    glm::vec3 segDir = camPos - prevCamPos;
    for (DirectedPortal& p : m_Portals)
        if (glm::dot(segDir, p.dir) > 0 && p.portal->ExactCheckSegIntersection(camPos, segDir))
            return p.portal->GetTransition(this);
    return this;
}
void Room::ReleaseGPUData()
{
    m_BVHVis.ReleaseGPUData();
    for (SuperMeshInstance* mesh : m_Meshes)
        delete mesh;
}