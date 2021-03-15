#include "ImmediateRenderer.h"
#include "Mesh.h"
#include "VertexData.h"
#include "GraphicsViewport.h"
#include "VertexFormatHelper.h"

ImmediateRenderer::ImmediateRenderer(GraphicsDevice& device) : m_LinesUsedVertexCount(0), m_PointsUsedVertexCount(0)
{
//#ifdef _DEBUG
    VertexFormat vertexFormat({ VertexFormatHelper::PurePosPropProto(0), VertexFormatHelper::PureColorPropProto(0) });
    m_VertexData = VertexData(vertexFormat, IMMEDIATE_RENDER_MAX_VERTEXES);
    m_VertexBuffer = VertexBuffer(device, m_VertexData, GraphicsBuffer::UsageFlag_Default);
    m_InputLayout = GraphicsInputLayout(device, vertexFormat, GetVertexShader(device));
    BasicVSConsts temp;
    m_ConstantsBuffer = GraphicsConstantsBuffer<BasicVSConsts>(device);

    m_PointsStartLocation = IMMEDIATE_RENDER_MAX_VERTEXES / 2;
//#endif
}

void ImmediateRenderer::OnFrameBegin(GraphicsDevice& device)
{
//#ifdef _DEBUG
    device.GetD3D11DeviceContext()->UpdateSubresource(m_VertexBuffer.GetBuffer(), 0, nullptr, m_VertexData.GetDataPtrForSlot(0), m_VertexData.GetSizeInBytesForSlot(0), 1);
    //m_Topology.UpdateVertexBuffers(device, m_VertexData);
    m_LinesUsedVertexCount = 0;
    m_PointsUsedVertexCount = 0;
//#endif
}
void ImmediateRenderer::OnFrameEnd(GraphicsDevice& device, Camera& camera, ColorSurface colorTarget, DepthSurface depthTarget)
{
//#ifdef _DEBUG
    GraphicsShader lineExpandGS = GetLineExpandShader(device);
    //GraphicsGeometryShader pointExpandGS = shadersCollection.GetShader<GraphicsGeometryShader>(std::wstring(L"Test/pointexpandgs.hlsl"), 0u);

    ID3D11RenderTargetView* colorTargetView = colorTarget.GetView();
    ID3D11DepthStencilView* depthView = depthTarget.GetView();
    device.GetD3D11DeviceContext()->OMSetRenderTargets(1, &colorTargetView, depthView);

    BasicVSConsts consts;
    consts.projection = camera.GetProjectionMatrix();
    consts.view = camera.GetViewMatrix();
    m_ConstantsBuffer.Update(device, consts);

    m_InputLayout.Bind(device);
    m_VertexBuffer.Bind(device);
    GetVertexShader(device).Bind(device);
    GetPixelShader(device).Bind(device);
    GetLineExpandShader(device).Bind(device);

    m_ConstantsBuffer.Bind(device, GraphicsShaderMask_Vertex | GraphicsShaderMask_Pixel | GraphicsShaderMask_Geometry);

    device.GetD3D11DeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    device.GetD3D11DeviceContext()->Draw(m_LinesUsedVertexCount, 0);

    GraphicsShader::Unbind(device, GraphicsShaderType_Geometry);
    GraphicsShader::Unbind(device, GraphicsShaderType_Pixel);
    GraphicsShader::Unbind(device, GraphicsShaderType_Vertex);
//#endif
    //m_Topology.DrawCallEx(device, lineExpandGS, Topology_Lines, m_LinesUsedVertexCount, 0);
    //m_Topology.DrawCallEx(device, pointExpandGS, Topology_Points, m_PointsUsedVertexCount, m_PointsStartLocation);
}

void ImmediateRenderer::Line(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& color)
{
    assert(m_LinesUsedVertexCount + 2 < m_PointsStartLocation);
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_LinesUsedVertexCount) = VertexType(v1, color);
    m_LinesUsedVertexCount++;
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_LinesUsedVertexCount) = VertexType(v2, color);
    m_LinesUsedVertexCount++;
}
void ImmediateRenderer::Line(const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color)
{
    Line(glm::vec4(v1, 1.0f), glm::vec4(v2, 1.0f), color);
}
void ImmediateRenderer::WireframeAABB(const AABB& aabb, const glm::vec4& color)
{
    //bottom plane
    Line(aabb.GetPoint(AABB::LeftBottomBack), aabb.GetPoint(AABB::LeftBottomFront), color);
    Line(aabb.GetPoint(AABB::LeftBottomBack), aabb.GetPoint(AABB::RightBottomBack), color);
    Line(aabb.GetPoint(AABB::RightBottomBack), aabb.GetPoint(AABB::RightBottomFront), color);
    Line(aabb.GetPoint(AABB::RightBottomFront), aabb.GetPoint(AABB::LeftBottomFront), color);

    //top plane
    Line(aabb.GetPoint(AABB::LeftTopBack), aabb.GetPoint(AABB::LeftTopFront), color);
    Line(aabb.GetPoint(AABB::LeftTopBack), aabb.GetPoint(AABB::RightTopBack), color);
    Line(aabb.GetPoint(AABB::RightTopBack), aabb.GetPoint(AABB::RightTopFront), color);
    Line(aabb.GetPoint(AABB::RightTopFront), aabb.GetPoint(AABB::LeftTopFront), color);

    //left plane
    Line(aabb.GetPoint(AABB::LeftBottomBack), aabb.GetPoint(AABB::LeftBottomFront), color);
    Line(aabb.GetPoint(AABB::LeftBottomBack), aabb.GetPoint(AABB::LeftTopBack), color);
    Line(aabb.GetPoint(AABB::LeftTopBack), aabb.GetPoint(AABB::LeftTopFront), color);
    Line(aabb.GetPoint(AABB::LeftTopFront), aabb.GetPoint(AABB::LeftBottomFront), color);

    //right plane
    Line(aabb.GetPoint(AABB::RightBottomBack), aabb.GetPoint(AABB::RightBottomFront), color);
    Line(aabb.GetPoint(AABB::RightBottomBack), aabb.GetPoint(AABB::RightTopBack), color);
    Line(aabb.GetPoint(AABB::RightTopBack), aabb.GetPoint(AABB::RightTopFront), color);
    Line(aabb.GetPoint(AABB::RightTopFront), aabb.GetPoint(AABB::RightBottomFront), color);
}
void ImmediateRenderer::Point(const glm::vec4& v, const glm::vec4& color)
{
    assert(m_PointsUsedVertexCount + 1 < IMMEDIATE_RENDER_MAX_VERTEXES);
    *(((VertexType*)m_VertexData.GetDataPtrForSlot(0)) + m_PointsUsedVertexCount + m_PointsStartLocation) = VertexType(v, color);
    m_PointsUsedVertexCount++;
}

void ImmediateRenderer::ReleaseGPUData(GraphicsDevice& device)
{
    m_InputLayout.ReleaseGPUData();
    m_VertexBuffer.ReleaseGPUData();
    GetVertexShader(device).ReleaseGPUData();
    GetPixelShader(device).ReleaseGPUData();
    GetLineExpandShader(device).ReleaseGPUData();
}