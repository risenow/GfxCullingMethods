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

enum ViewportVisibility
{
    ViewportVisibility_LinearFrustum,
    ViewportVisibility_BVHFrustum,
    ViewportVisibility_OctreeFrustum,
    ViewportVisibility_None
};

class SuperViewport
{
public:
    SuperViewport(const GraphicsViewport& viewport, Camera& camera, ViewportVisibility visibility) : m_Viewport(viewport), m_Camera(camera)
    {
        switch (visibility)
        {
        case ViewportVisibility_LinearFrustum:
            m_Visibility =  new LinearFrustumVisibility({});
            break;
        case ViewportVisibility_BVHFrustum:
            m_Visibility = new BVHFrustumVisibility({});
            break;
        case ViewportVisibility_OctreeFrustum:
            m_Visibility = new OctreeFrustumVisibility({});
            break;
        case ViewportVisibility_None:
        default:
            m_Visibility = nullptr;
            break;
        }
    }

    void AddMeshes(const std::vector<SuperMeshInstance*>& meshes, bool applyVisibility = true, bool rebuildVisibility = false)
    {
        if (applyVisibility && m_Visibility)
        {
            m_Visibility->AddMeshes(meshes, rebuildVisibility);
            return;
        }
        m_Meshes.reserve(m_Meshes.size() + meshes.size());
        m_Meshes.insert(m_Meshes.end(), meshes.begin(), meshes.end());
    }
    void AddMesh(SuperMeshInstance* mesh, bool applyVisibility = true, bool rebuildVisibility = false)
    {
        if (applyVisibility && m_Visibility)
        {
            m_Visibility->AddMesh(mesh, rebuildVisibility);
            return;
        }
        m_Meshes.push_back(mesh);
    }
    void ShareVisibility(const std::vector<SuperViewport*>& viewports)
    {
        for (SuperViewport* viewport : viewports)
            viewport->m_Visibility = m_Visibility;
    }
    void SetVisHelpInfo(AABB sceneAABB)
    {
        m_Visibility->SetSceneAABB(sceneAABB);
    }
    Visibility* GetVisibility() // to delete thish method since it does not match design intentions
    {
        return m_Visibility;
    }

    RenderStatistics Render(GraphicsDevice& device, ColorSurface colorTarget, DepthSurface depthTarget, bool clearTargets = true)
    {
        m_Viewport.Bind(device);
        ID3D11RenderTargetView* colorTargetView = colorTarget.GetView();
        ID3D11DepthStencilView* depthView = depthTarget.GetView();
        device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &colorTargetView, depthView);

        if (clearTargets)
        {
            glm::vec4 clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
            device.GetD3D11DeviceContext()->ClearRenderTargetView(colorTargetView, (float*)&clearColor.data);
            device.GetD3D11DeviceContext()->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
        std::vector<SuperMeshInstance*> visibleMeshes; // to make it class-global for allocations number reduce

        MicrosecondsTimer timer;
        timer.Begin();
        m_Visibility->Gather(device, m_Camera, visibleMeshes);
        int64_t est = timer.End();

        RenderStatistics stats;
        for (SuperMeshInstance* mesh : visibleMeshes)
            stats += mesh->Render(device, m_Camera);
        for (SuperMeshInstance* mesh : m_Meshes)
            stats += mesh->Render(device, m_Camera);

        return stats;
    }

    void ReleaseGPUData()
    {
        m_Visibility->Clear();
        for (SuperMeshInstance* mesh : m_Meshes)
            delete mesh;
    }
private:
    GraphicsViewport m_Viewport;
    Camera& m_Camera;
    std::vector<SuperMeshInstance*> m_Meshes;
    Visibility* m_Visibility;
};