#include "GraphicsConstantsBuffer.h"
#include "GraphicsShader.h"

void BindMultipleGraphicsConstantBuffers(GraphicsDevice& device, index_t startSlotIndex, const std::vector<GraphicsBuffer*>& constantBuffers, GraphicsShaderMaskType stageMask)
{
	assert(stageMask != 0);

	void(__stdcall ID3D11DeviceContext::*setConstantBuffers[GraphicsShaderType_Count])(UINT, UINT, ID3D11Buffer*const*);
	setConstantBuffers[GraphicsShaderType_Vertex] = &ID3D11DeviceContext::VSSetConstantBuffers;
	setConstantBuffers[GraphicsShaderType_Pixel] = &ID3D11DeviceContext::PSSetConstantBuffers;
    setConstantBuffers[GraphicsShaderType_Compute] = &ID3D11DeviceContext::CSSetConstantBuffers;
    setConstantBuffers[GraphicsShaderType_Hull] = &ID3D11DeviceContext::HSSetConstantBuffers;
    setConstantBuffers[GraphicsShaderType_Domain] = &ID3D11DeviceContext::DSSetConstantBuffers;
	setConstantBuffers[GraphicsShaderType_Geometry] = &ID3D11DeviceContext::GSSetConstantBuffers;

	std::vector<ID3D11Buffer*> buffers(constantBuffers.size());
	for (unsigned long i = 0; i < constantBuffers.size(); i++)
	{
		buffers[i] = (ID3D11Buffer*)constantBuffers[i]->GetBuffer();
	}

	for (unsigned long i = 0; i < GraphicsShaderType_Count; i++)
	{
		if (stageMask & (1u<<i))
			(device.GetD3D11DeviceContext()->*setConstantBuffers[i])(startSlotIndex, constantBuffers.size(), buffers.data());
	}
}