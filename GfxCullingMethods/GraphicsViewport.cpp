#include "GraphicsViewport.h"

GraphicsViewport::GraphicsViewport() : m_Width(0), m_Height(0), m_MinDepth(0.0f), m_MaxDepth(1.0f)
{
    FillD3D11ViewportStructure();
}
GraphicsViewport::GraphicsViewport(const Window& window, NumberRange<float> depthRange) :
                                     m_Width(window.GetWidth()), m_Height(window.GetHeight()), 
                                     m_MinDepth(depthRange.GetStart()), m_MaxDepth(depthRange.GetEnd())
{
    FillD3D11ViewportStructure();
}


/*GraphicsViewport::GraphicsViewport(const RenderSet& renderSet, NumberRange<float> depthRange)
                                   : m_TopLeft(0, 0), 
                                     m_Width(renderSet.GetWidth()), m_Height(renderSet.GetHeight()),
                                     m_MinDepth(depthRange.GetStart()), m_MaxDepth(depthRange.GetEnd())
{
    FillD3D11ViewportStructure();
}*/
GraphicsViewport::GraphicsViewport(const ColorSurface& surface, NumberRange<float> depthRange)
                                   : m_TopLeft(0, 0),
                                     m_Width(surface.GetWidth()), m_Height(surface.GetHeight()),
                                     m_MinDepth(depthRange.GetStart()), m_MaxDepth(depthRange.GetEnd())
{
    FillD3D11ViewportStructure();
}
GraphicsViewport::GraphicsViewport(const BoundRect& bounds, NumberRange<float> depthRange)
                                   : m_TopLeft(bounds.m_TopLeft), 
                                     m_Width(bounds.m_Width), m_Height(bounds.m_Height),
                                     m_MinDepth(depthRange.GetStart()), m_MaxDepth(depthRange.GetEnd())
{
    FillD3D11ViewportStructure();
}

void GraphicsViewport::FillD3D11ViewportStructure()
{
    m_D3D11Viewport.TopLeftX = (float)m_TopLeft.m_X;
    m_D3D11Viewport.TopLeftY = (float)m_TopLeft.m_Y;
    m_D3D11Viewport.Width = (float)m_Width;
    m_D3D11Viewport.Height = (float)m_Height;
    m_D3D11Viewport.MinDepth = m_MinDepth;
    m_D3D11Viewport.MaxDepth = m_MaxDepth;
}

void GraphicsViewport::Bind(GraphicsDevice& device)
{
    device.GetD3D11DeviceContext()->RSSetViewports(1, &m_D3D11Viewport);
}