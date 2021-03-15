#include "GraphicsShader.h"
#include "dxlogicsafety.h"
#include "Log.h"
#include "logicsafety.h"
#include "strutils.h"

std::string GetShaderD3DTarget(GraphicsShaderType shaderType, const std::string& targetVersionStr)
{
	return ShaderPerTypeTargetNamePrefixes[shaderType] + targetVersionStr;
}

std::string GetShaderEntryPointName(GraphicsShaderType type)
{
	return ShaderPerTypeEntryPointNamePrefixes[type] + ShaderEntryPointNameBase;
}

void CreateVSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
	device.GetD3D11Device()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11VertexShader**)shader);
}
void CreatePSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
	device.GetD3D11Device()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11PixelShader**)shader);
}
void CreateHSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
	device.GetD3D11Device()->CreateHullShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11HullShader**)shader);
}
void CreateDSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
	device.GetD3D11Device()->CreateDomainShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11DomainShader**)shader);
}
void CreateGSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
	device.GetD3D11Device()->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11GeometryShader**)shader);
}
void CreateCSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader)
{
    device.GetD3D11Device()->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, (ID3D11ComputeShader**)shader);
}

void BindVSShader(GraphicsDevice& device, ID3D11DeviceChild* vertexShader)
{
	device.GetD3D11DeviceContext()->VSSetShader((ID3D11VertexShader*)vertexShader, nullptr, 0);
}
void BindPSShader(GraphicsDevice& device, ID3D11DeviceChild* pixelShader)
{
	device.GetD3D11DeviceContext()->PSSetShader((ID3D11PixelShader*)pixelShader, nullptr, 0);
}
void BindCSShader(GraphicsDevice& device, ID3D11DeviceChild* computeShader)
{
    device.GetD3D11DeviceContext()->CSSetShader((ID3D11ComputeShader*)computeShader, nullptr, 0);
}
void BindHSShader(GraphicsDevice& device, ID3D11DeviceChild* hullShader)
{
    device.GetD3D11DeviceContext()->HSSetShader((ID3D11HullShader*)hullShader, nullptr, 0);
}
void BindDSShader(GraphicsDevice& device, ID3D11DeviceChild* domainShader)
{
    device.GetD3D11DeviceContext()->DSSetShader((ID3D11DomainShader*)domainShader, nullptr, 0);
}
void BindGSShader(GraphicsDevice& device, ID3D11DeviceChild* geometryShader)
{
	device.GetD3D11DeviceContext()->GSSetShader((ID3D11GeometryShader*)geometryShader, nullptr, 0);
}

void GetD3DShaderMacros(const std::vector<GraphicsShaderMacro>& inShaderMacros, std::vector<D3D_SHADER_MACRO>& outD3DShaderMacros)
{
	outD3DShaderMacros.resize(inShaderMacros.size());
	for (index_t i = 0; i < inShaderMacros.size(); i++)
	{
		outD3DShaderMacros[i].Name = inShaderMacros[i].m_Name.c_str();
		outD3DShaderMacros[i].Definition = inShaderMacros[i].m_Value.c_str();
	}
	D3D_SHADER_MACRO nullMacro;
	memset((void*)&nullMacro, 0, sizeof(nullMacro));
	outD3DShaderMacros.push_back(nullMacro);
}