#pragma once
#include <vector>
#include "AABB.h"
#include "SuperMeshInstance.h"
#include "BVHFrustumVisibility.h"
#include "OctreeFrustumVisibility.h"
#include "Camera.h"
#include "LoPoApproxGeom.h"

namespace PortalSystem
{
    void GenPortalFrustum(Camera::Frustum& prevFr, Camera::Frustum& fr, const glm::vec3& camPos, const glm::vec3& rightV, const glm::vec3& topV, const AABB& portal);

    class Room;
    class Portal
    {
    public:
        Portal();
        Portal(const LoPoApproxGeom& approxGeom, Room* room1, Room* room2);

        Room* GetTransition(Room* from);

        bool Valid();
        void GatherVisibleObjects(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Room* from, std::vector<std::vector<SuperMeshInstance*>>& meshInstancesLists, size_t meshInstsListIndex = 0);
        bool ExactCheckSegIntersection(const glm::vec3& orig, const glm::vec3& dir);
        AABB GetAABB();
    private:
        Room* m_Rooms[2];
        LoPoApproxGeom m_PortalGeom;
    };

    class Room
    {
    public:
        struct DirectedPortal
        {
            Portal* portal;
            glm::vec3 dir;
        };

        Room();
        void GatherVisibleObjects(GraphicsDevice& dev, Camera& cam, Camera::Frustum& actualFrustum, Portal* from, std::vector<std::vector<SuperMeshInstance*>>& meshInstancesLists, size_t meshInstsListIndex = 0);
        void AddMesh(SuperMeshInstance* mesh, bool applyVis = true, bool rebuildVis = false);
        void AddPortal(Portal* portal, const glm::vec3& dir );

        Room* RoomTransition(const glm::vec3& prevCamPos, const glm::vec3& camPos);

        void ReleaseGPUData();
    private:
        std::vector<DirectedPortal> m_Portals;
        
        std::vector<SuperMeshInstance*> m_Meshes; //additional geometry not managed by visibility systems
        BVHFrustumVisibility m_BVHVis;
    };
}