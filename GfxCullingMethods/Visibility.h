#pragma once
#include "GraphicsDevice.h"
#include "Camera.h"
#include "SuperMeshInstance.h"
#include <vector>

class Visibility
{
public:
    virtual void Gather(GraphicsDevice& device, Camera& cam, std::vector<SuperMeshInstance*>& instances) = 0;
    virtual void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool rebuild = false) = 0;
    virtual void AddMesh(SuperMeshInstance* meshes, bool rebuild = false) = 0;
    virtual void SetSceneAABB(const AABB& aabb) {}
    virtual void Rebuild() = 0;
    virtual void Clear() = 0;
};