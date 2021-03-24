#pragma once
#include <vector>
#include "AABB.h"
#include "SuperMeshInstance.h"
#include "BVHFrustumVisibility.h"
#include "Camera.h"
#include "LoPoApproxGeom.h"
#include "SuperViewport.h"

namespace PortalSystem
{
    void GenPortalFrustum(Camera::Frustum& prevFr, Camera::Frustum& fr, const glm::vec3& camPos, const glm::vec3& rightV, const glm::vec3& topV, const AABB& portal);//shouldnt be there
    class Room;
    class Portal
    {
    public:
        Portal();
        Portal(const LoPoApproxGeom& approxGeom, Room* room1, Room* room2)
        {
            m_Rooms[0] = room1;
            m_Rooms[1] = room2;

            m_PortalGeom = approxGeom;
        }

        bool Valid();
        void GatherVisibleObjects(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<SuperMeshInstance*>& meshInstances);
        bool ExactCheckSegIntersection(const glm::vec3& orig, const glm::vec3& dir);
        AABB GetAABB();
    private:
        Room* m_Rooms[2];
        //AABB m_AABB;
        LoPoApproxGeom m_PortalGeom;
    };

    class Room
    {
    public:
        Room();
        void GatherVisibleObjects(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<SuperMeshInstance*>& meshInstances);
        void AddMesh(SuperMeshInstance* mesh, bool rebuildVis = false);
        void AddPortal(Portal* portal);
        void ShareVis(SuperViewport& vp)
        {
            vp.m_Visibility = &m_BVHVis;
        }
    private:
        std::vector<Portal*> m_Portals;
        BVHFrustumVisibility m_BVHVis;
    };
}