#include "GraphicsBuffer.h"
#include "dxlogicsafety.h"
#include "logicsafety.h"

unsigned int BindFlagsToD3D11BindFlags(size_t bindFlags)
{
    unsigned int flags = 0;
    if (bindFlags & GraphicsBuffer::BindFlag_Vertex)
        flags |= D3D11_BIND_VERTEX_BUFFER;
    if (bindFlags & GraphicsBuffer::BindFlag_Index)
        flags |= D3D11_BIND_INDEX_BUFFER;
    if (bindFlags & GraphicsBuffer::BindFlag_Constant)
        flags |= D3D11_BIND_CONSTANT_BUFFER;
    if (bindFlags & GraphicsBuffer::BindFlag_Shader)
        flags |= D3D11_BIND_SHADER_RESOURCE;
    if (bindFlags & GraphicsBuffer::BindFlag_UAV)
        flags |= D3D11_BIND_UNORDERED_ACCESS;


    return flags;
}

void CalculateD3D11UsageAndCPUAccessFlags(const GraphicsBuffer::UsageFlags usageFlags, D3D11_USAGE& d3d11UsageFlags, unsigned int& d3d11CPUAccessFlags)
{
    switch (usageFlags)
    {
    case GraphicsBuffer::UsageFlag_Default:
        d3d11UsageFlags = D3D11_USAGE_DEFAULT;
        d3d11CPUAccessFlags = 0;
        break;
    case GraphicsBuffer::UsageFlag_Immutable:
        d3d11UsageFlags = D3D11_USAGE_IMMUTABLE;
        d3d11CPUAccessFlags = 0;
        break;
    case GraphicsBuffer::UsageFlag_Dynamic:
        d3d11UsageFlags = D3D11_USAGE_DYNAMIC;
        d3d11CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        break;
    case GraphicsBuffer::UsageFlag_Staging:
        d3d11UsageFlags = D3D11_USAGE_STAGING;
        d3d11CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
        break;
    default:
        popAssert(false);
        break;
    }
}

unsigned int MiscFlagToD3D11MiscFlag(size_t miscFlag)
{
    size_t flags = 0;
    if (miscFlag & GraphicsBuffer::MiscFlag_Structured)
        flags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    if (miscFlag & GraphicsBuffer::MiscFlag_DrawIndirectArgs)
        flags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    if (miscFlag & GraphicsBuffer::MiscFlag_Raw)
        flags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

    return flags;
}

void CreateDesc(const size_t size, size_t bindFlag,
                const GraphicsBuffer::UsageFlags usageFlag, size_t miscFlag, void* data, size_t structureByteStride,
                D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA& resourceData)
{
    bufferDesc.ByteWidth = size;
    bufferDesc.BindFlags = BindFlagsToD3D11BindFlags(bindFlag);
    bufferDesc.MiscFlags = MiscFlagToD3D11MiscFlag(miscFlag);
    bufferDesc.StructureByteStride = structureByteStride;

    CalculateD3D11UsageAndCPUAccessFlags(usageFlag, bufferDesc.Usage, bufferDesc.CPUAccessFlags);

    resourceData.pSysMem = data;
    resourceData.SysMemPitch = 0;
    resourceData.SysMemSlicePitch = 0;
}

void CreateSRVDesc(size_t size, size_t numElements, size_t structureByteStride, DXGI_FORMAT format, D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	desc.Format = format;

	desc.Buffer.ElementOffset = 0;
    desc.Buffer.ElementWidth = numElements;//structureByteStride ? structureByteStride : size;
}

GraphicsBuffer::GraphicsBuffer(GraphicsDevice& device, size_t size, size_t bindFlag,
                                UsageFlags usageFlag, size_t miscFlag/* = MiscFlag_Default*/,
                                void* data/* = nullptr*/, size_t numElements, size_t structureByteStride/* = 0*/, DXGI_FORMAT format/* = DXGI_FORMAT_UNKNOWN*/) : m_SRV(nullptr), m_Buffer(nullptr)
{
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_SUBRESOURCE_DATA subresourceData;

    CreateDesc(size, bindFlag, usageFlag, miscFlag, data, structureByteStride, bufferDesc, subresourceData);
    D3D_HR_OP(device.GetD3D11Device()->CreateBuffer(&bufferDesc, (data) ? &subresourceData : nullptr, (ID3D11Buffer**)&m_Buffer));

    if (bindFlag != BindFlag_Vertex && bindFlag != BindFlag_Constant && bindFlag != BindFlag_Index && !(miscFlag & MiscFlag_DrawIndirectArgs))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        CreateSRVDesc(size, numElements, structureByteStride, format, srvDesc);
        D3D_HR_OP(device.GetD3D11Device()->CreateShaderResourceView(m_Buffer, &srvDesc, (ID3D11ShaderResourceView * *)& m_SRV));
    }
}
void GraphicsBuffer::Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask) 
{
	assert(stageMask != 0);

	void(__stdcall ID3D11DeviceContext::*setShaderResources[GraphicsShaderType_Count])(UINT, UINT, ID3D11ShaderResourceView*const*);
	setShaderResources[GraphicsShaderType_Vertex] = &ID3D11DeviceContext::VSSetShaderResources;
	setShaderResources[GraphicsShaderType_Pixel] = &ID3D11DeviceContext::PSSetShaderResources;
    setShaderResources[GraphicsShaderType_Domain] = &ID3D11DeviceContext::DSSetShaderResources;

	for (unsigned long i = 0; i < GraphicsShaderType_Count; i++)
	{
		if (stageMask & (1u << i))
			(device.GetD3D11DeviceContext()->*setShaderResources[i])(0, 1, (ID3D11ShaderResourceView**)&m_SRV);
	}
}

VertexBuffer::VertexBuffer() : m_VertexSizeInBytes(0) {}
VertexBuffer::VertexBuffer(GraphicsDevice& device, VertexData& vertexData, GraphicsBuffer::UsageFlags flags, index_t slotIndex) : GraphicsBuffer(device, 
                                                                                                  vertexData.GetSizeInBytesForSlot(slotIndex),
                                                                                                  BindFlag_Vertex, flags,
                                                                                                  MiscFlag_Default, vertexData.GetDataPtrForSlot(slotIndex)),
                                                                                                  m_VertexSizeInBytes(vertexData.GetVertexFormat().GetVertexSizeInBytesForSlot(slotIndex))
{
    //popAssert(vertexData.GetVertexFormat().GetNumSlotsUsed() == 1); // vertex buffer per slot

}
void VertexBuffer::Bind(GraphicsDevice& device)
{
	BindVertexBuffers(device, { *this });
    unsigned int offset = 0;
    device.GetD3D11DeviceContext()->IASetVertexBuffers(0, 1, (ID3D11Buffer**)&m_Buffer, &m_VertexSizeInBytes, (unsigned int*)&offset);
}

size_t VertexBuffer::GetVertexSizeInBytes() const
{
	return m_VertexSizeInBytes;
}

//use _malloca instead?
void BindVertexBuffers(GraphicsDevice& device, const std::vector<VertexBuffer>& vertexBuffers)
{
	UINT* offsets = (UINT*)alloca(sizeof(UINT)*vertexBuffers.size());
	unsigned int* sizesPerVertex = (unsigned int*)alloca(sizeof(size_t)*vertexBuffers.size());
	ID3D11Buffer** buffers = (ID3D11Buffer**)alloca(sizeof(ID3D11Buffer*)*vertexBuffers.size());
	for (unsigned long i = 0; i < vertexBuffers.size(); i++)
	{
		offsets[i] = 0;
		sizesPerVertex[i] = vertexBuffers[i].GetVertexSizeInBytes();
		buffers[i] = (ID3D11Buffer*)vertexBuffers[i].GetBuffer();
	}

	device.GetD3D11DeviceContext()->IASetVertexBuffers(0, vertexBuffers.size(), buffers, sizesPerVertex, offsets);
}

IndexBuffer::IndexBuffer() {}
IndexBuffer::IndexBuffer(GraphicsDevice& device, const std::vector<uint32_t>& indexes, GraphicsBuffer::UsageFlags flags) : GraphicsBuffer(device,
    sizeof(uint32_t) * indexes.size(),
    BindFlag_Index, flags,
    MiscFlag_Default, (void*)indexes.data())
{
}
void IndexBuffer::Bind(GraphicsDevice& device)
{
    unsigned int offset = 0;
    if (m_Buffer)
        device.GetD3D11DeviceContext()->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}



