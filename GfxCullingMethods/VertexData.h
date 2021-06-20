#pragma once
#include <string>
#include <vector>
#include <d3d11.h>
#include <dxgi.h>
#include "glm/glm.hpp"
#include "typesalias.h"
#include "logicsafety.h"
#include "arrayutil.h"
#include <vector>

enum VertexDataSemantics
{
	VertexDataSemantics_PurePos,
	VertexDataSemantics_PureColor,
	VertexDataSemantics_PureNormal,
	VertexDataSemantics_PureTexCoord,
	VertexDataSemantics_Count
};
static const std::string VertexDataSemanticsStrs[] = { "POSITION", "COLOR", "NORMAL", "TEXCOORD"};
static_assert(popGetArraySize(VertexDataSemanticsStrs) == VertexDataSemantics_Count, "VertexDataSemanticsStrs array should represent every record from enum VertexDataSemantics");

template<class T> // glm type
DXGI_FORMAT GetVertexDXGIFormat()
{
	popAssert(false);
	return DXGI_FORMAT_UNKNOWN;
}
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec2>();
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec3>();
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec4>();
template<> // 
DXGI_FORMAT GetVertexDXGIFormat<uint32_t>();


struct VertexPropertyPrototype
{
	VertexPropertyPrototype();

	//std::string m_Name;
	size_t m_SemanticCode;
	size_t m_Size;

	index_t m_SlotIndex;
	index_t m_SemanticIndex;
	DXGI_FORMAT m_DXGIFormat;

	 bool operator == (const VertexPropertyPrototype& other) const
	{
		return m_SemanticCode == other.m_SemanticCode && m_Size == other.m_Size && m_SlotIndex == other.m_SlotIndex && m_SemanticIndex == other.m_SemanticIndex && m_DXGIFormat == other.m_DXGIFormat;
	}
};

template<class T> //glm type
VertexPropertyPrototype CreateVertexPropertyPrototype(VertexDataSemantics semCode, index_t semanticIndex = 0, index_t slotIndex = 0)
{
	VertexPropertyPrototype vertexPropertyPrototype;
	vertexPropertyPrototype.m_SemanticCode = semCode;
	vertexPropertyPrototype.m_SemanticIndex = semanticIndex;
	vertexPropertyPrototype.m_SlotIndex = slotIndex;
	vertexPropertyPrototype.m_DXGIFormat = GetVertexDXGIFormat<T>();
	vertexPropertyPrototype.m_Size = sizeof(T);
	return vertexPropertyPrototype;
}

struct VertexProperty
{
	VertexProperty();
	VertexProperty(const VertexPropertyPrototype& prototype, offset_t bytesOffset);

	VertexPropertyPrototype m_Prototype;
	offset_t m_BytesOffset;

	bool operator == (const VertexProperty& other) const
	{
		return m_BytesOffset == other.m_BytesOffset && m_Prototype == other.m_Prototype;
	}
};

struct VertexFormat
{
    VertexFormat();
	VertexFormat(const std::vector<VertexPropertyPrototype>& vertexPropertyPrototypes);

    count_t GetNumSlotsUsed() const;
    size_t GetVertexSizeInBytesForSlot(index_t slotIndex) const;
	void AddVertexProperty(const VertexPropertyPrototype& vertexPropertyPrototype);

	bool HasSemantic(index_t semanticIndex);

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3D11InputElementDescs() const;

    std::vector<size_t> m_PerSlotVertexSizes;
    std::vector<VertexProperty> m_VertexProperties;

	bool operator==(const VertexFormat& other) const
	{
		return m_NumSlots == other.m_NumSlots && m_PerSlotVertexSizes == other.m_PerSlotVertexSizes && m_VertexProperties == other.m_VertexProperties;
	}
private:
    count_t m_NumSlots;
};

class VertexData
{
public:
    VertexData();
    VertexData(const VertexFormat& format, count_t vertexNum, count_t instancesCount = 1);

    size_t GetSizeInBytesForSlot(index_t slotIndex) const;
    count_t GetNumVertexes() const;
    VertexProperty GetVertexPropertyByName(VertexDataSemantics semCode, index_t semanticIndex = 0) const;
    VertexFormat GetVertexFormat() const;

    void* GetDataPtrForSlot(index_t slotIndex);
    void* GetVertexPropertyDataPtrForVertexWithIndex(index_t index, const VertexProperty& vertexProperty);
	void* GetVertexPropertyDataPtr(const VertexProperty& vertexProperty);

    void SetIndexes(const std::vector<uint32_t>& indexes);
    std::vector<uint32_t>& GetIndexes();

	bool HasNormals();
	bool HasTexCoords();

	void Resize(count_t vertexNum);
protected:
    VertexFormat m_VertexFormat;
    count_t m_NumVertexes;
    count_t m_NumSlots;

    typedef std::vector<byte_t> RawData;
    typedef std::vector<RawData> VertexSlotsData;
    VertexSlotsData m_Data;
    std::vector<uint32_t> m_Indexes;
};

template<class T>
void PullVecToVertexData(size_t vxIndex, T v, VertexData& data, VertexProperty prop)
{
	T* p = (T*)data.GetVertexPropertyDataPtrForVertexWithIndex(vxIndex, prop);
	(*p) = v;
}