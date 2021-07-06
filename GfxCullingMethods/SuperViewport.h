#pragma once
#include "GraphicsViewport.h"
#include "Camera.h"
#include "SuperMeshInstance.h"
#include "GraphicsSurface.h"
#include "Visibility.h"
#include "LinearFrustumVisibility.h"
#include "BVHFrustumVisibility.h"
#include "OctreeFrustumVisibility.h"
#include "OctreeFrustumVisibility.h"
#include "MicrosecondsTimer.h"
#include "Scene.h"

enum ViewportVisibility
{
    ViewportVisibility_LinearFrustum,
    ViewportVisibility_BVHFrustum,
    ViewportVisibility_OctreeFrustum,
    ViewportVisibility_None
};

namespace PortalSystem
{
    class Room;
};

class SuperViewport
{
public:
    SuperViewport(const GraphicsViewport& viewport, Camera& camera, Scene* scene);

    void ConsumeScene(Scene* scene);

    RenderStatistics Render(GraphicsDevice& device, ColorSurface colorTarget, DepthSurface depthTarget, bool clearTargets = true);
    void Update();
    void ReleaseGPUData();

    void AddMeshDbg(SuperMeshInstance* mesh);
private:
    Scene* m_Scene;
    GraphicsViewport m_Viewport;
    Camera& m_Camera;

    std::vector<SuperMeshInstance*> m_ViewportMeshes; // debug puposes only
};