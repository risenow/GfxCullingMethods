#pragma once
#include <d3d11.h>
#include "Window.h"
#include "GraphicsSurface.h"
#include "GraphicsDevice.h"
#include "helperutil.h"
#include "areahelper.h"

class GraphicsViewport
{
public:
    GraphicsViewport();
    GraphicsViewport(const Window& window, NumberRange<float> depthRange = NumberRange<float>(0.0f, 1.0f));
    //GraphicsViewport(const RenderSet& renderSet, NumberRange<float> depthRange = NumberRange<float>(0.0f, 1.0f));
    GraphicsViewport(const ColorSurface& surface, NumberRange<float> depthRange = NumberRange<float>(0.0f, 1.0f));
    GraphicsViewport(const BoundRect& bounds, NumberRange<float> depthRange = NumberRange<float>(0.0, 1.0f));

    void Bind(GraphicsDevice& device);
private:
    void FillD3D11ViewportStructure();

    D3D11_VIEWPORT m_D3D11Viewport;

    size_t m_Width;
    size_t m_Height;

    Point2D m_TopLeft;

    float m_MinDepth;
    float m_MaxDepth;
};