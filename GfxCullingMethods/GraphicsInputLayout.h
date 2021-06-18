#pragma once
#include "GraphicsDevice.h"
#include "VertexData.h"
#include "GraphicsShader.h"

class GraphicsInputLayout
{
public:
	GraphicsInputLayout();
    GraphicsInputLayout(GraphicsDevice& device, const VertexFormat& vertexFormat, const GraphicsShader& shader);

	void Bind(GraphicsDevice& device);

    void ReleaseGPUData()
    {
        m_InputLayout->Release();
        m_InputLayout = nullptr;
    }

	ID3D11InputLayout* GetD3D11InputLayout();
private:
    ID3D11InputLayout* m_InputLayout;
};