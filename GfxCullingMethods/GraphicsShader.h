#pragma once
#include <string>
#include <vector>
#include <limits>
#include <d3dcompiler.h>
#include "GraphicsDevice.h"
#include "dxlogicsafety.h"
#include "strutils.h"
#include "typesalias.h"
#include "arrayutil.h"

void CreateVSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);
void CreatePSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);
void CreateHSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);
void CreateDSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);
void CreateGSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);
void CreateCSShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DeviceChild** shader);

enum GraphicsShaderType { GraphicsShaderType_Vertex = 0,
						  GraphicsShaderType_Pixel,
                          GraphicsShaderType_Compute,
                          GraphicsShaderType_Hull,
                          GraphicsShaderType_Domain,
						  GraphicsShaderType_Geometry,
                          GraphicsShaderType_Invalid };
enum GraphicsShaderMask { GraphicsShaderMask_Vertex = (1u << GraphicsShaderType_Vertex),
						  GraphicsShaderMask_Pixel  = (1u << GraphicsShaderType_Pixel),
                          GraphicsShaderMask_Compute  = (1u << GraphicsShaderType_Compute),
                          GraphicsShaderMask_Hull = (1u << GraphicsShaderType_Hull),
                          GraphicsShaderMask_Domain = (1u << GraphicsShaderType_Domain),
						  GraphicsShaderMask_Geometry = (1u << GraphicsShaderType_Geometry)
};
typedef unsigned long GraphicsShaderMaskType;

static const unsigned long GraphicsShaderType_Count = GraphicsShaderType_Invalid;

static const std::string ShaderEntryPointNameBase = "Entry";
static const std::string ShaderPerTypeEntryPointNamePrefixes[] = { "VS", "PS", "CS", "HS", "DS", "GS"};
static const std::string ShaderPerTypeTargetNamePrefixes[] = { "vs_", "ps_", "cs_", "hs_", "ds_", "gs_" };
static_assert(popGetArraySize(ShaderPerTypeEntryPointNamePrefixes) == popGetArraySize(ShaderPerTypeTargetNamePrefixes), "");


template<class T>
struct ShaderTypeIndex
{
	static const index_t value = GraphicsShaderType_Invalid;
};
template<>
struct ShaderTypeIndex<ID3D11VertexShader>
{
	static const index_t value = GraphicsShaderType_Vertex;
};
template<>
struct ShaderTypeIndex<ID3D11PixelShader>
{
	static const index_t value = GraphicsShaderType_Pixel;
};
template<>
struct ShaderTypeIndex<ID3D11ComputeShader>
{
    static const index_t value = GraphicsShaderType_Compute;
};
template<>
struct ShaderTypeIndex<ID3D11HullShader>
{
    static const index_t value = GraphicsShaderType_Hull;
};
template<>
struct ShaderTypeIndex<ID3D11DomainShader>
{
    static const index_t value = GraphicsShaderType_Domain;
};
template<>
struct ShaderTypeIndex<ID3D11GeometryShader>
{
	static const index_t value = GraphicsShaderType_Geometry;
};

template<class T>
index_t GetShaderTypeIndex()
{
	index_t shaderTypeIndex = ShaderTypeIndex<T>::value;
	popAssert(shaderTypeIndex != GraphicsShaderType_Invalid);
	return shaderTypeIndex;
}

std::string GetShaderD3DTarget(GraphicsShaderType shaderType, const std::string& targetVersionStr = "5_0");
std::string GetShaderEntryPointName(GraphicsShaderType type);


void BindVSShader(GraphicsDevice& device, ID3D11DeviceChild* vertexShader);
void BindPSShader(GraphicsDevice& device, ID3D11DeviceChild* pixelShader);
void BindCSShader(GraphicsDevice& device, ID3D11DeviceChild* computeShader);
void BindHSShader(GraphicsDevice& device, ID3D11DeviceChild* hullShader);
void BindDSShader(GraphicsDevice& device, ID3D11DeviceChild* domainShader);
void BindGSShader(GraphicsDevice& device, ID3D11DeviceChild* domainShader);

struct GraphicsShaderMacro
{
	GraphicsShaderMacro(const std::string& name, const std::string& value) : m_Name(name), m_Value(value)
	{}

	std::string m_Name;
	std::string m_Value;
};


typedef std::vector<GraphicsShaderMacro> ShaderVariation;

struct ExtendedShaderVariation
{
	ShaderVariation m_ShaderVariation;
	uint32_t m_Bits;
};

void GetD3DShaderMacros(const std::vector<GraphicsShaderMacro>& inShaderMacros, std::vector<D3D_SHADER_MACRO>& outD3DShaderMacros);

class GraphicsShader
{
private:
	static const index_t BLOB_INDEX = 0;
	static const index_t SHADER_INDEX = 1;

#define popBlobDX11OBject m_DX11Objects[BLOB_INDEX]
#define popShaderDX11Object m_DX11Objects[SHADER_INDEX]

public:
	GraphicsShader();
	GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {});
	GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::string& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {});

	static GraphicsShader CreateFromTextContent(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& filePath, const std::string& content, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {});
	static GraphicsShader FromFile(GraphicsDevice& device, GraphicsShaderType type, const std::wstring& filePath, const ShaderVariation& var = {});

	virtual ~GraphicsShader();

	void Bind(GraphicsDevice& device);
	static void Unbind(GraphicsDevice& device, GraphicsShaderType type);

	ID3DBlob* GetBlob() const;
	ID3D11DeviceChild* GetShader() const;
	GraphicsShaderType GetType() const;
	ID3DBlob*& GetBlobRef();
	ID3D11DeviceChild*& GetShaderRef();

	bool IsValid() const;

	static GraphicsShader GetEmpty();

	void ReleaseGPUData();
private:
	GraphicsShaderType m_Type;
    ID3DBlob* m_Blob;
    ID3D11DeviceChild* m_Shader;
    size_t m_VariationID; //????
};

void GetAllMacrosCombinations(const std::vector<GraphicsShaderMacro>& macroSet, std::vector<std::vector<GraphicsShaderMacro>>& permutations, size_t hasAnyOfRule = 0, size_t hasOnlyOneOfRule = 0, size_t optionallyHasOnlyOneOfRule = 0, size_t hasAllOfRule = 0);
std::vector<ExtendedShaderVariation> GetAllPermutations(const std::vector<GraphicsShaderMacro>& macroSet, size_t hasAnyOfRule = 0, size_t hasOnlyOneOfRule = 0, size_t optionallyHasOnlyOneOfRule = 0, size_t hasAllOfRule = 0);
