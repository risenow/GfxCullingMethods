#pragma once
#include "GraphicsDevice.h"
#include "GraphicsBuffer.h"
#include "GraphicsShader.h"

void BindMultipleGraphicsConstantBuffers(GraphicsDevice& device, index_t startSlotIndex, const std::vector<GraphicsBuffer*>& constantBuffers, GraphicsShaderMaskType stageMask);

template<class T>
class GraphicsConstantsBuffer : public GraphicsBuffer //mb rename to ConstantsBuffer(as VertexBuffer)
{
public:
	GraphicsConstantsBuffer() {}
	GraphicsConstantsBuffer(GraphicsDevice& device) : GraphicsBuffer(device, sizeof(T), GraphicsBuffer::BindFlag_Constant, GraphicsBuffer::UsageFlag_Dynamic, GraphicsBuffer::MiscFlag_Default)
	{}
	GraphicsConstantsBuffer(GraphicsDevice& device, T& consts) : GraphicsBuffer(device, sizeof(T), GraphicsBuffer::BindFlag_Constant, GraphicsBuffer::UsageFlag_Dynamic, GraphicsBuffer::MiscFlag_Default, &consts)
	{}

	void Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask, index_t startSlotIndex = 0)
	{
		BindMultipleGraphicsConstantBuffers(device, startSlotIndex, { this }, stageMask);
	}
	void Update(GraphicsDevice& device, T constants)
	{
		//need a perfomance test vs map unmap seq
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		device.GetD3D11DeviceContext()->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &constants, sizeof(T));
		device.GetD3D11DeviceContext()->Unmap(m_Buffer, 0);
		//device.GetD3D11DeviceContext()->UpdateSubresource((ID3D11Resource*)GetBuffer(), 0, nullptr, &constants, sizeof(constants), 0);
	}
private:
};


