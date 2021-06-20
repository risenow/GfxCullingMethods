#pragma once
#include "GraphicsDevice.h"
#include "VertexData.h"
#include "GraphicsShader.h"

class GraphicsBuffer
{
private:
	static const index_t BUFFER_INDEX = 0;
	static const index_t SHADER_RESOURCE_INDEX = 1;

public:
    enum BindFlags { BindFlag_Vertex = 1 << 0, BindFlag_Index = 1 << 1, BindFlag_Constant = 1 << 2, BindFlag_Shader = 1 << 3, BindFlag_UAV = 1 << 4};
    enum UsageFlags { UsageFlag_Dynamic, UsageFlag_Staging, UsageFlag_Immutable, UsageFlag_Default };
    enum MiscFlags { MiscFlag_Default = 0, MiscFlag_Structured = 1 << 0, MiscFlag_DrawIndirectArgs = 1 << 1, MiscFlag_Raw = 1 << 2 };

	GraphicsBuffer() : m_Buffer(nullptr), m_SRV(nullptr) {}
    GraphicsBuffer(GraphicsDevice& device, size_t size, size_t bindFlag, 
                   UsageFlags usageFlag, size_t miscFlag = MiscFlag_Default, 
                   void* data = nullptr, size_t numElements = 0, size_t structureByteStride = 0, DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN);

	void Bind(GraphicsDevice& device, GraphicsShaderMaskType stageMask);

    ID3D11ShaderResourceView* GetSRV() const
    {
        return m_SRV;
    }
    ID3D11Buffer* GetBuffer() const
    {
        return m_Buffer;
    }

    void ReleaseGPUData() 
    {
        if (m_SRV)
            m_SRV->Release(); 
        m_SRV = nullptr;

        m_Buffer->Release();
        m_Buffer = nullptr;
    }
	virtual ~GraphicsBuffer()
	{
	}
protected:
    ID3D11Buffer* m_Buffer;
    ID3D11ShaderResourceView* m_SRV;
    //void CreateDesc(const size_t size, const BindFlags bindFlag, 
    //                const UsageFlags usageFlag, const MiscFlags miscFlag, void* data, size_t structureByStride, 
    //                D3D11_BUFFER_DESC& bufferDesc, D3D11_SUBRESOURCE_DATA& resourceData);
};

class VertexBuffer : public GraphicsBuffer
{
public:
	VertexBuffer();
    VertexBuffer(GraphicsDevice& device, VertexData& vertexData, UsageFlags usage = UsageFlag_Immutable, index_t slotIndex = 0);

    void Bind(GraphicsDevice& device);
	size_t GetVertexSizeInBytes() const;
private:
    unsigned int m_VertexSizeInBytes;
};

class IndexBuffer : public GraphicsBuffer
{
public:
    IndexBuffer();
    IndexBuffer(GraphicsDevice& device, const std::vector<uint32_t>& indexes, UsageFlags usage = UsageFlag_Immutable);

    void Bind(GraphicsDevice& device);
private:
};

void BindVertexBuffers(GraphicsDevice& device, const std::vector<VertexBuffer>& vertexBuffers);

template<class T>
class StructuredBuffer : public GraphicsBuffer
{
public:
    StructuredBuffer() {}
    StructuredBuffer(GraphicsDevice& device, const std::vector<T>& objs, UsageFlags usage = UsageFlag_Default) : GraphicsBuffer(device,
        sizeof(T)* objs.size(),
        BindFlag_Shader,
        usage,
        MiscFlag_Structured, (void*)objs.data(), objs.size(), sizeof(T))
    {}
};

template<class T>
class RWStructuredBuffer : public GraphicsBuffer
{
public:
    RWStructuredBuffer() {}
    RWStructuredBuffer(GraphicsDevice& device, const std::vector<T>& objs, UsageFlags usage = UsageFlag_Default) : GraphicsBuffer(device,
        sizeof(T)* objs.size(),
        BindFlag_Shader | BindFlag_UAV, usage,
        MiscFlag_Structured, (void*)objs.data(), objs.size(), sizeof(T)) 
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        ZeroMemory(&uavDesc, sizeof(uavDesc));
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = objs.size();
        uavDesc.Buffer.Flags = 0;
        uavDesc.Buffer.FirstElement = 0;
        
        D3D_HR_OP(device.GetD3D11Device()->CreateUnorderedAccessView(m_Buffer, &uavDesc, (ID3D11UnorderedAccessView**)&m_UAV));
        
    }
    ID3D11UnorderedAccessView* GetUAV() 
    {
        return m_UAV;
    }
private:
    ID3D11UnorderedAccessView* m_UAV;
};


template<class T>
class RWByteAddressBuffer : public GraphicsBuffer
{
public:
    RWByteAddressBuffer() {}
    RWByteAddressBuffer(GraphicsDevice& device, const std::vector<T>& objs, size_t miscFlag, UsageFlags usage = UsageFlag_Default) : GraphicsBuffer(device,
        sizeof(T)* objs.size(),
        BindFlag_Shader | BindFlag_UAV, usage,
        miscFlag | MiscFlag_Raw, (void*)objs.data())
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        ZeroMemory(&uavDesc, sizeof(uavDesc));
        uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.NumElements = objs.size()*(sizeof(T)/4);
        uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
        uavDesc.Buffer.FirstElement = 0;

        D3D_HR_OP(device.GetD3D11Device()->CreateUnorderedAccessView(m_Buffer, &uavDesc, (ID3D11UnorderedAccessView**)&m_UAV));

    }
    ID3D11UnorderedAccessView* GetUAV()
    {
        return m_UAV;
    }
private:
    ID3D11UnorderedAccessView* m_UAV;
};