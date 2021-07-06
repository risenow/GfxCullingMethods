#include "GraphicsShader.h"
#include "dxlogicsafety.h"
#include "Log.h"
#include "logicsafety.h"
#include "helperutil.h"
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

void GetAllMacrosCombinations(const std::vector<GraphicsShaderMacro>& macroSet, std::vector<ExtendedShaderVariation>& permutations, size_t hasAnyOfRule, size_t hasOnlyOneOfRule, size_t optionallyHasOnlyOneOfRule, size_t hasAllOfRule)
{
    size_t setBitmask = pow(2, macroSet.size()) - 1;
    for (size_t i = 0; i <= setBitmask; i++)
    {
        if (optionallyHasOnlyOneOfRule && !((i & optionallyHasOnlyOneOfRule) == 0 || isExpOf2(i & optionallyHasOnlyOneOfRule)))
            continue;
        if (hasOnlyOneOfRule && !isExpOf2(i & hasOnlyOneOfRule))
            continue;
        if (hasAnyOfRule && (i & hasAnyOfRule) == 0)
            continue;
        if (hasAllOfRule && (i & hasAllOfRule) != hasAllOfRule)
            continue;
        permutations.push_back(ExtendedShaderVariation());
        ShaderVariation& currentMutation = permutations.back().m_ShaderVariation;
        for (size_t j = 0; j < macroSet.size(); j++)
        {
            if (i & (1 << j))
                currentMutation.push_back(macroSet[j]);
        }
        permutations.back().m_Bits = i;
    }
}
std::vector<ExtendedShaderVariation> GetAllPermutations(const std::vector<GraphicsShaderMacro>& macroSet, size_t hasAnyOfRule, size_t hasOnlyOneOfRule, size_t optionallyHasOnlyOneOfRule, size_t hasAllOfRule)
{
    std::vector<ExtendedShaderVariation> mutations;
    GetAllMacrosCombinations(macroSet, mutations, hasAnyOfRule, hasOnlyOneOfRule, optionallyHasOnlyOneOfRule, hasAllOfRule);

    return mutations;
}

GraphicsShader::GraphicsShader() : m_Shader(nullptr)
{}
GraphicsShader::GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros)
{
	std::string narrowFileName = wstrtostr(fileName);
	*this = GraphicsShader(device, shaderType, narrowFileName, graphicsShaderMacros);
}
GraphicsShader::GraphicsShader(GraphicsDevice& device, GraphicsShaderType shaderType, const std::string& fileName, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros) : m_Type(shaderType)
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

GraphicsShader GraphicsShader::CreateFromTextContent(GraphicsDevice& device, GraphicsShaderType shaderType, const std::wstring& filePath, const std::string& content, const std::vector<GraphicsShaderMacro>& graphicsShaderMacros)
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
		entry.c_str(), target.c_str(), D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR, 0,
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

GraphicsShader GraphicsShader::FromFile(GraphicsDevice& device, GraphicsShaderType type, const std::wstring& filePath, const ShaderVariation& var)
{
	std::ifstream shaderFile(filePath);
	std::stringstream buffer;
	buffer << shaderFile.rdbuf();

	std::string content = buffer.str();

	return CreateFromTextContent(device, type, filePath, content, var);
}

GraphicsShader::~GraphicsShader()
{
	//ReleaseDX11Objects();
}

void GraphicsShader::Bind(GraphicsDevice& device)
{
	void(*bindShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3D11DeviceChild * shader) = { BindVSShader,
																										  BindPSShader,
																										  BindCSShader,
																										  BindHSShader,
																										  BindDSShader,
																										  BindGSShader };
	bindShader[m_Type](device, GetShader());
}

void GraphicsShader::Unbind(GraphicsDevice& device, GraphicsShaderType type)
{
	void(*bindShader[GraphicsShaderType_Count])(GraphicsDevice & device, ID3D11DeviceChild * shader) = { BindVSShader,
																										  BindPSShader,
																										  BindCSShader,
																										  BindHSShader,
																										  BindDSShader,
																										  BindGSShader };
	bindShader[type](device, nullptr);
}

ID3DBlob* GraphicsShader::GetBlob() const
{
	return (ID3DBlob*)m_Blob;
}

ID3D11DeviceChild* GraphicsShader::GetShader() const
{
	return m_Shader;
}

GraphicsShaderType GraphicsShader::GetType() const
{
	return m_Type;
}

ID3DBlob*& GraphicsShader::GetBlobRef()
{
	return m_Blob;
}

ID3D11DeviceChild*& GraphicsShader::GetShaderRef()
{
	return m_Shader;
}

bool GraphicsShader::IsValid() const
{
	return (m_Shader != nullptr);
}

GraphicsShader GraphicsShader::GetEmpty()
{
	return GraphicsShader();
}

void GraphicsShader::ReleaseGPUData()
{
	m_Blob->Release();
	m_Blob = nullptr;

	m_Shader->Release();
	m_Shader = nullptr;
}
