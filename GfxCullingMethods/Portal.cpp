#include "Portal.h"
#include "glm/gtx/norm.hpp"

using namespace PortalSystem;

void PortalSystem::GenPortalFrustum(Camera::Frustum& prevFr, Camera::Frustum& fr, const glm::vec3& camPos, const glm::vec3& rightV, const glm::vec3& topV, const AABB& portal, glm::vec3& rt, glm::vec3& tp)
{
    int maxXIndex = -1; float maxXScore = -FLT_MAX;
    int maxYIndex = -1; float maxYScore = -FLT_MAX;
    int minXIndex = -1; float minXScore = FLT_MAX;
    int minYIndex = -1; float minYScore = FLT_MAX;
    
    glm::vec3 front = portal.m_Centroid - camPos;
    glm::vec3 right = -glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 top = glm::cross(front, right);

    rt = right;
    tp = top;

    for (size_t i = 0; i < 8; i++)
    {
        glm::vec3 p = portal.GetPoint(i);
        float xScore = glm::dot(p, right);//prevFr.planes[0].Test(p);
        float yScore = glm::dot(p, top);//prevFr.planes[3].Test(p);

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
    glm::vec3 minXP = portal.GetPoint(minXIndex);//portal.m_Max;//m_Min;//portal.GetPoint(minXIndex);
    glm::vec3 maxXP = portal.GetPoint(maxXIndex);//portal.m_Min;//m_Max;//portal.GetPoint(maxXIndex);
    glm::vec3 minYP = portal.GetPoint(minYIndex);//portal.m_Min;//portal.GetPoint(minYIndex);
    glm::vec3 maxYP = portal.GetPoint(maxYIndex);//portal.m_Max;//portal.GetPoint(maxYIndex);

    glm::vec3 rightVec = right; //glm::normalize(prevFr.planes[0].n);
    glm::vec3 topVec = top;//glm::normalize(prevFr.planes[3].n);
    glm::vec3 n;

    //left plane
    n = glm::cross(minXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    /*if (glm::dot(n, rightVec) > 0.0f)
        fr.planes[0].n = n;
    else
        fr.planes[0].n = -n;*/
    fr.planes[0].n = -n;
    fr.planes[0].d = -glm::dot(fr.planes[0].n, minXP);

    //right plane
    n = glm::cross(maxXP - camPos, glm::vec3(0.0f, 1.0f, 0.0f));
    /*if (glm::dot(n, rightVec) > 0.0f)
        fr.planes[1].n = -n;
    else
        fr.planes[1].n = n;*///-n;
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
    glm::vec3 c = -glm::cross(prevLeftPlN_XZ, leftPlN_XZ);//glm::cross(leftPlN_XZ, prevLeftPlN_XZ);
    if (glm::dot(c, top) > 0.0f || glm::length2(prevLeftPlN_XZ) < 0.0001) ///2nd cond !!!! 
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
    if (glm::dot(c, top) > 0.0f || glm::length2(prevRightPlN_XZ) < 0.0001) ///2nd cond !!!! 
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
    c = glm::cross(prevBotPlN_XZ, botPlN_XZ);//glm::cross(leftPlN_XZ, prevLeftPlN_XZ);
    if (glm::dot(c, right) > 0.0f || glm::length2(prevBotPlN_XZ) < 0.0001) ///2nd cond !!!! 
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
    c = -glm::cross(prevTopPlN_XZ, topPlN_XZ);//glm::cross(leftPlN_XZ, prevLeftPlN_XZ);
    if (glm::dot(c, right) > 0.0f || glm::length2(prevTopPlN_XZ) < 0.0001) ///2nd cond !!!! 
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
    //cull planes against prev fr
    /*glm::vec3 leftPlN = glm::normalize(fr.planes[0].n);
    glm::vec3 rightPlN = glm::normalize(fr.planes[1].n);
    glm::vec3 prevLeftPlN = glm::normalize(prevFr.planes[0].n);
    glm::vec3 prevRightPlN = glm::normalize(prevFr.planes[1].n);
    glm::vec3 topPlN = glm::normalize(fr.planes[2].n);
    //cull xz planes
    glm::vec3 c = glm::cross(prevLeftPlN, leftPlN);
    float v = glm::dot(c, top);
    float sign = glm::sign(v);
    if (sign > 0.0f || FEQUAL(v, 0.0f))
        fr.planes[0] = prevFr.planes[0];
    else
    { }*/
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
    static const glm::vec3 epsOffset = glm::vec3(0.001, 0.001, 0.001);

    AABB aabb = m_PortalGeom.CalcAABB();
    aabb.m_Max += epsOffset;
    aabb.m_Min -= epsOffset;

    return aabb;
}
void Portal::GatherVisibleObjects(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<SuperMeshInstance*>& meshInstances)
{
    Camera::Frustum camFrust = Camera::Frustum(cam);
    Camera::Frustum cutFrustum = Camera::Frustum(cam);
    glm::vec3 d;
    GenPortalFrustum(camFrust, cutFrustum, cam.GetPosition(), cam.GetRightVec(), cam.GetTopVec(), GetAABB(), d, d);

    for (Room* room : m_Rooms)
        if (room != from)
            room->GatherVisibleObjects(device, cam, cutFrustum, this, meshInstances);
}
bool Portal::ExactCheckSegIntersection(const glm::vec3& orig, const glm::vec3& dir)
{
    return m_PortalGeom.IntersectSeg(orig, dir);
}
Room::Room() : m_BVHVis({}) {}
void Room::GatherVisibleObjects(GraphicsDevice& device, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<SuperMeshInstance*>& meshInstances)
{
    m_BVHVis.GatherFrustum(device, actualFrustum, meshInstances);

    for (Portal* portal : m_Portals)
        if (portal != from && actualFrustum.Test(portal->GetAABB()))
            portal->GatherVisibleObjects(device, cam, actualFrustum, this, meshInstances);
}
void Room::AddMesh(SuperMeshInstance* mesh, bool rebuildsVis)
{
    m_BVHVis.AddMesh(mesh, rebuildsVis);
}

void Room::AddPortal(Portal* portal)
{
    m_Portals.push_back(portal);
}