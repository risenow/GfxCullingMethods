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
/*void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11VertexShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11PixelShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11HullShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11DomainShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11GeometryShader** shader);
void CreateShader(GraphicsDevice& device, ID3DBlob* blob, ID3D11ComputeShader** shader);
*/
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
/*void BindShader(GraphicsDevice& device, ID3D11VertexShader* vertexShader);
void BindShader(GraphicsDevice& device, ID3D11PixelShader* pixelShader);
void BindShader(GraphicsDevice& device, ID3D11ComputeShader* computeShader);
void BindShader(GraphicsDevice& device, ID3D11HullShader* hullShader);
void BindShader(GraphicsDevice& device, ID3D11DomainShader* domainShader);
void BindShader(GraphicsDevice& device, ID3D11GeometryShader* domainShader);*/

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
	GraphicsShader() : m_Shader(nullptr)
	{}
	GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {})
	{
		std::string narrowFileName = wstrtostr(fileName);
		*this = GraphicsShader(device, shaderType, narrowFileName, graphicsShaderMacros);
	}
	GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::string& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {}) : m_Type(shaderType)
	{
		void(*createShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3DBlob * blob, ID3D11DeviceChild * *shader) = { CreateVSShader,
																													   CreatePSShader,
																													   CreateCSShader,
																													   CreateHSShader,
																													   CreateDSShader,
																													   CreateGSShader };
		ID3DBlob* errorMsgs = nullptr;

		std::vector<D3D_SHADER_MACRO> shaderMacros;
		GetD3DShaderMacros(graphicsShaderMacros, shaderMacros);

		//D3D_HR_OP(D3DCompileFromFile(strtowstr(fileName).c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
		//							 GetShaderEntryPointName<T>().c_str(), GetShaderD3DTarget<T>().c_str(), D3DCOMPILE_PREFER_FLOW_CONTROL | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		//							 (ID3DBlob**)&popBlobDX11OBject.GetRef(), &errorMsgs));
		if (errorMsgs)
		{
			LOG_FILE(std::string((char*)errorMsgs->GetBufferPointer()));
            std::string serror = std::string((char*)errorMsgs->GetBufferPointer());
			errorMsgs->Release();
		}

        std::string target = GetShaderD3DTarget(shaderType);
        std::string entryPoint = GetShaderEntryPointName(shaderType).c_str();

		createShader[shaderType](device, (ID3DBlob*)m_Blob, &m_Shader);
#ifdef _DEBUG
		GetShader()->SetPrivateData(WKPDID_D3DDebugObjectName, fileName.size(), fileName.c_str());
#endif
	}

	static GraphicsShader CreateFromTextContent(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& filePath, const std::string& content, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros = {})
	{
		void(*createShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3DBlob * blob, ID3D11DeviceChild * *shader) = { CreateVSShader,
																													   CreatePSShader,
																													   CreateCSShader,
																													   CreateHSShader,
																													   CreateDSShader,
																													   CreateGSShader };

		GraphicsShader shader;
		shader.m_Type = shaderType;
		ID3DBlob* errorMsgs = nullptr;

		std::vector<D3D_SHADER_MACRO> shaderMacros;
		GetD3DShaderMacros(graphicsShaderMacros, shaderMacros);

		D3D_SHADER_MACRO* macrosTest = shaderMacros.data();
		macrosTest++;
        std::string target = GetShaderD3DTarget(shaderType);
        std::string entry = GetShaderEntryPointName(shaderType);

		D3D_HR_OP(D3DCompile(content.c_str(), content.size(), wstrtostr_fast(filePath).c_str(), shaderMacros.data(), D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entry.c_str(), target.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS |D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0,
			(ID3DBlob**)&shader.GetBlobRef(), &errorMsgs));
		if (errorMsgs)
		{
			LOG(std::string((char*)errorMsgs->GetBufferPointer()));
			errorMsgs->Release();
		}

		createShader[shaderType](device, (ID3DBlob*)shader.GetBlob(), &shader.GetShaderRef());
#ifdef _DEBUG
		//shader.GetShader()->SetPrivateData(WKPDID_D3DDebugObjectName, fileName.size(), fileName.c_str());
#endif

		return shader;
	}

	static GraphicsShader FromFile(GraphicsDevice& device, GraphicsShaderType type, const std::wstring& filePath, const ShaderVariation& var = {})
	{
		std::ifstream shaderFile(filePath);
		std::stringstream buffer;
		buffer << shaderFile.rdbuf();

		std::string content = buffer.str();

		return CreateFromTextContent(device, type, filePath, content, var);
	}

	virtual ~GraphicsShader()
	{
		//ReleaseDX11Objects();
	}

	void Bind(GraphicsDevice& device)
	{
		void(*bindShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3D11DeviceChild *shader) = {   BindVSShader,
																											  BindPSShader,
																											  BindCSShader,
																											  BindHSShader,
																											  BindDSShader,
																											  BindGSShader };
		bindShader[m_Type](device, GetShader());
	}

	static void Unbind(GraphicsDevice& device, GraphicsShaderType type)
	{
		void(*bindShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3D11DeviceChild * shader) = { BindVSShader,
																											  BindPSShader,
																											  BindCSShader,
																											  BindHSShader,
																											  BindDSShader,
																											  BindGSShader };
		bindShader[type](device, nullptr);
	}

    ID3DBlob* GetBlob() const
	{
		return (ID3DBlob*)m_Blob;
	}

	ID3D11DeviceChild* GetShader() const
	{
		return m_Shader;
	}

	GraphicsShaderType GetType() const
	{
		return m_Type;
	}

    ID3DBlob*& GetBlobRef()
    {
        return m_Blob;
    }

    ID3D11DeviceChild*& GetShaderRef()
    {
        return m_Shader;
    }

	bool IsValid() const
	{
		return (m_Shader != nullptr);
	}

	static GraphicsShader GetEmpty()
	{
		return GraphicsShader();
	}

    void ReleaseGPUData()
    {
        m_Blob->Release();
        m_Blob = nullptr;

        m_Shader->Release();
        m_Shader = nullptr;
    }
private:
	GraphicsShaderType m_Type;
    ID3DBlob* m_Blob;
    ID3D11DeviceChild* m_Shader;
    size_t m_VariationID; //????
};

void GetAllMacrosCombinations(const std::vector<GraphicsShaderMacro>& macroSet, std::vector<std::vector<GraphicsShaderMacro>>& permutations, size_t hasAnyOfRule = 0, size_t hasOnlyOneOfRule = 0, size_t optionallyHasOnlyOneOfRule = 0, size_t hasAllOfRule = 0);
std::vector<ExtendedShaderVariation> GetAllPermutations(const std::vector<GraphicsShaderMacro>& macroSet, size_t hasAnyOfRule = 0, size_t hasOnlyOneOfRule = 0, size_t optionallyHasOnlyOneOfRule = 0, size_t hasAllOfRule = 0);
