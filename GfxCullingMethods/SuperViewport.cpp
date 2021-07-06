#include "SuperViewport.h"

SuperViewport::SuperViewport(const GraphicsViewport& viewport, Camera& camera, Scene* scene) : m_Viewport(viewport), m_Camera(camera), m_Scene(scene)
{
}

void SuperViewport::ConsumeScene(Scene* scene)
{
    assert(scene);
    m_Scene = scene;
}

RenderStatistics SuperViewport::Render(GraphicsDevice& device, ColorSurface colorTarget, DepthSurface depthTarget, bool clearTargets)
{
    m_Viewport.Bind(device);
    ID3D11RenderTargetView* colorTargetView = colorTarget.GetView();
    ID3D11DepthStencilView* depthView = depthTarget.GetView();
    device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &colorTargetView, depthView);

    if (clearTargets)
    {
        glm::vec4 clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0);
        device.GetD3D11DeviceContext()->ClearRenderTargetView(colorTargetView, (float*)&clearColor);
        device.GetD3D11DeviceContext()->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    RenderStatistics stats;
    if (m_Scene)
        stats = m_Scene->Render(device, m_Camera);
    for (SuperMeshInstance* mesh : m_ViewportMeshes)
        stats += mesh->Render(device, m_Camera);

    return stats;
}
void SuperViewport::Update()
{
    if (m_Scene)
        m_Scene->Update(m_Camera);
}
void SuperViewport::ReleaseGPUData()
{
    if (m_Scene)
        m_Scene->ReleaseGPUData();
}

void SuperViewport::AddMeshDbg(SuperMeshInstance* mesh)
{
    m_ViewportMeshes.push_back(mesh);
}