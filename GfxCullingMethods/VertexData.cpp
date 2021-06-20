#include "VertexData.h"
#include "logicsafety.h"

template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec2>()
{
    return DXGI_FORMAT_R32G32_FLOAT;
}
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec3>()
{
	return DXGI_FORMAT_R32G32B32_FLOAT;
}
template<> // glm type
DXGI_FORMAT GetVertexDXGIFormat<glm::vec4>()
{
	return DXGI_FORMAT_R32G32B32A32_FLOAT;
}
template<> // 
DXGI_FORMAT GetVertexDXGIFormat<uint32_t>()
{
	return DXGI_FORMAT_R32_UINT;
}

D3D11_INPUT_ELEMENT_DESC GetD3D11InputElementDesc(const VertexProperty& vertexProperty)
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc;
	inputElementDesc.SemanticName      = VertexDataSemanticsStrs[vertexProperty.m_Prototype.m_SemanticCode].c_str();
	inputElementDesc.SemanticIndex     = vertexProperty.m_Prototype.m_SemanticIndex;
	inputElementDesc.Format            = vertexProperty.m_Prototype.m_DXGIFormat;
	inputElementDesc.InputSlot = vertexProperty.m_Prototype.m_SlotIndex;
	inputElementDesc.AlignedByteOffset = vertexProperty.m_BytesOffset;
	inputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc.InstanceDataStepRate = 0;

	return inputElementDesc;
}


VertexPropertyPrototype::VertexPropertyPrototype() : m_Size(0), m_SlotIndex(0), m_SemanticIndex(0) {}

VertexProperty::VertexProperty() : m_BytesOffset(0) {}
VertexProperty::VertexProperty(const VertexPropertyPrototype& vertexPropertyPrototype, offset_t bytesOffset) : m_Prototype(vertexPropertyPrototype), m_BytesOffset(bytesOffset) {}

VertexFormat::VertexFormat() : m_NumSlots(0) {}
VertexFormat::VertexFormat(const std::vector<VertexPropertyPrototype>& vertexPropertyPrototypes) : m_NumSlots(0)
{
	for (const VertexPropertyPrototype& vertexPropertyPrototype : vertexPropertyPrototypes)
	{
		AddVertexProperty(vertexPropertyPrototype);
	}
}
count_t VertexFormat::GetNumSlotsUsed() const
{
    return m_NumSlots;
}
size_t VertexFormat::GetVertexSizeInBytesForSlot(index_t slotIndex) const
{
    return m_PerSlotVertexSizes[slotIndex];
}
void VertexFormat::AddVertexProperty(const VertexPropertyPrototype& vertexPropertyPrototype)
{
	index_t slotIndex = vertexPropertyPrototype.m_SlotIndex;

    if (m_NumSlots < slotIndex + 1)
    {
        m_NumSlots = slotIndex + 1;
        m_PerSlotVertexSizes.resize(m_NumSlots);
        m_PerSlotVertexSizes[slotIndex] = 0;
    }

	m_VertexProperties.push_back(VertexProperty(vertexPropertyPrototype, m_PerSlotVertexSizes[slotIndex]));
    m_PerSlotVertexSizes[slotIndex] += vertexPropertyPrototype.m_Size;
}
std::vector<D3D11_INPUT_ELEMENT_DESC> VertexFormat::GetD3D11InputElementDescs() const
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs;
	for (index_t i = 0; i < m_VertexProperties.size(); i++)
	{
		inputElementDescs.push_back(GetD3D11InputElementDesc(m_VertexProperties[i]));
	}

	return inputElementDescs;
}
bool VertexFormat::HasSemantic(index_t sem)
{
    for (VertexProperty& prop : m_VertexProperties)
        if (prop.m_Prototype.m_SemanticIndex == sem)
            return true;
}

VertexData::VertexData() : m_NumVertexes(0)
{}
VertexData::VertexData(const VertexFormat& vertexFormat, count_t vertexesNum, count_t instancesCount) : m_VertexFormat(vertexFormat), m_NumVertexes(vertexesNum)
{
    m_NumSlots = m_VertexFormat.GetNumSlotsUsed();
    m_Data.resize(m_NumSlots);
    for (int i = 0; i < m_NumSlots; i++)
    {
        m_Data[i].resize(m_NumVertexes * m_VertexFormat.GetVertexSizeInBytesForSlot(i));
    }
}
size_t VertexData::GetSizeInBytesForSlot(index_t slotIndex) const
{
    return m_Data[slotIndex].size();
}
count_t VertexData::GetNumVertexes() const
{
    return m_NumVertexes;
}
VertexProperty VertexData::GetVertexPropertyByName(VertexDataSemantics semCode, index_t semanticIndex/* = 0*/) const
{
    for (int i = 0; i < m_VertexFormat.m_VertexProperties.size(); i++)
    {
        const VertexProperty& vertexProperty = m_VertexFormat.m_VertexProperties.at(i);
        if (vertexProperty.m_Prototype.m_SemanticCode == semCode && vertexProperty.m_Prototype.m_SemanticIndex == semanticIndex)
            return vertexProperty;
    }
    popAssert(false);
	return VertexProperty();
}
VertexFormat VertexData::GetVertexFormat() const
{
    return m_VertexFormat;
}
void* VertexData::GetDataPtrForSlot(index_t slotIndex)
{
    return (void*)m_Data[slotIndex].data();
}
void* VertexData::GetVertexPropertyDataPtrForVertexWithIndex(index_t index, const VertexProperty& vertexProperty)
{
    index_t byteIndex = index * m_VertexFormat.GetVertexSizeInBytesForSlot(vertexProperty.m_Prototype.m_SlotIndex) + vertexProperty.m_BytesOffset;
    return &m_Data[vertexProperty.m_Prototype.m_SlotIndex][byteIndex];
}

void* VertexData::GetVertexPropertyDataPtr(const VertexProperty& vertexProperty)
{
	return GetVertexPropertyDataPtrForVertexWithIndex(0, vertexProperty);
}
bool VertexData::HasNormals()
{
    return m_VertexFormat.HasSemantic(VertexDataSemantics_PureNormal);
}
bool VertexData::HasTexCoords()
{
    return m_VertexFormat.HasSemantic(VertexDataSemantics_PureTexCoord);
}
void VertexData::SetIndexes(const std::vector<uint32_t>& indexes)
{
    m_Indexes = indexes;
}

std::vector<uint32_t>& VertexData::GetIndexes()
{
    return m_Indexes;
}

void VertexData::Resize(count_t vertexNum)
{
	m_NumVertexes = vertexNum;
	for (int i = 0; i < m_NumSlots; i++)
	{
		m_Data[i].resize(m_NumVertexes * m_VertexFormat.GetVertexSizeInBytesForSlot(i));
	}
}