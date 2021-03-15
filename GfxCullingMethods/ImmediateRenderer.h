#pragma once
#include "glm/gtx/common.hpp"
#include "GraphicsBuffer.h"
#include "GraphicsInputLayout.h"
#include "GraphicsShader.h"
#include "GraphicsConstantsBuffer.h"
#include "VertexData.h"
#include "GraphicsSurface.h"
#include "Camera.h"
#include "basicvsconstants.h"

//#include "Data/Shaders/Test/basicvsconstants.h"

#define IMMEDIATE_RENDER_MAX_VERTEXES 500000

struct BasicPSConstsDummy { float __p1; float __p2; float __p3; float __p4; };

//use for debug purposes only(at least in current state)
class ImmediateRenderer
{
public:
    struct VertexType
    {
        VertexType() {}
        VertexType(const glm::vec4& pos, const glm::vec4& color) : m_Position(pos), m_Color(color) {}
        glm::vec4 m_Position;
        glm::vec4 m_Color;
    };
    static GraphicsShader& GetLineExpandShader(GraphicsDevice& dev)
    {
        static GraphicsShader s = GraphicsShader::FromFile(dev, GraphicsShaderType_Geometry, L"lineexpandgs.hlsl");
        return s;
    }
    static GraphicsShader& GetVertexShader(GraphicsDevice& dev)
    {
        static GraphicsShader s = GraphicsShader::FromFile(dev, GraphicsShaderType_Vertex, L"colorvs.hlsl");
        return s;
    }
    static GraphicsShader& GetPixelShader(GraphicsDevice& dev)
    {
        static GraphicsShader s = GraphicsShader::FromFile(dev, GraphicsShaderType_Pixel, L"colorps.hlsl");
        return s;
    }
    ImmediateRenderer();
    ImmediateRenderer(GraphicsDevice& device);
    void OnFrameBegin(GraphicsDevice& device);
    void OnFrameEnd(GraphicsDevice& device, Camera& camera, ColorSurface colorSurface, DepthSurface depthSurface); //render is here
    void Line(const glm::vec4& v1, const glm::vec4& v2, const glm::vec4& color);
    void Line(const glm::vec3& v1, const glm::vec3& v2, const glm::vec4& color);
    void WireframeAABB(const AABB& aabb, const glm::vec4& color);
    void Point(const glm::vec4& v, const glm::vec4& color);
    void ReleaseGPUData(GraphicsDevice& device);
    //void Render();
private:
    void RenderLines();

    size_t m_PointsStartLocation;
    VertexData m_VertexData;
    size_t m_LinesUsedVertexCount;
    size_t m_PointsUsedVertexCount;

    GraphicsConstantsBuffer<BasicVSConsts> m_ConstantsBuffer;
    VertexBuffer m_VertexBuffer;
    GraphicsInputLayout m_InputLayout;
};