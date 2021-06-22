#include "Texture2D.h"
#include "multisampleutils.h"
#include "logicsafety.h"
#include "dxlogicsafety.h"
#include <d3dcommon.h>

Texture2D::Texture2D() : m_Texture(nullptr) {}

DXGI_FORMAT GetFormatSRV(DXGI_FORMAT format, UINT bindFlags)
{
    if (format == DXGI_FORMAT_R24G8_TYPELESS)
        return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    return format;
}
Texture2D::Texture2D(GraphicsDevice& device, ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv) : m_Texture(texture), m_SRV(srv), m_UAV(nullptr)
{
    popAssert(texture);

    D3D11_TEXTURE2D_DESC desc;
    GetD3D11Texture2D()->GetDesc(&desc);

    m_Width = desc.Width;
    m_Height = desc.Height;
    m_ArraySize = desc.ArraySize;
    m_MipLevels = desc.MipLevels;
    m_DXGIFormat = desc.Format;
    m_BindFlags = desc.BindFlags;
    m_CPUAccessFlags = desc.CPUAccessFlags;
    m_MiscFlags = desc.MiscFlags;
    m_Usage = desc.Usage;
    m_SampleDesc = desc.SampleDesc;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Format = GetFormatSRV(m_DXGIFormat, m_BindFlags);
    srvDesc.ViewDimension = m_SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Texture2D.MipSlice = 0;
    uavDesc.Format = GetFormatSRV(m_DXGIFormat, m_BindFlags);//m_DXGIFormat;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    if (!srv && (m_BindFlags & D3D11_BIND_SHADER_RESOURCE))
        device.GetD3D11Device()->CreateShaderResourceView((ID3D11Texture2D*)(m_Texture), (D3D11_SHADER_RESOURCE_VIEW_DESC*)& srvDesc, (ID3D11ShaderResourceView * *)& m_SRV);
    if (!(m_BindFlags & D3D11_BIND_DEPTH_STENCIL) && (m_BindFlags & D3D11_BIND_UNORDERED_ACCESS))
        device.GetD3D11Device()->CreateUnorderedAccessView((ID3D11Texture2D*)(m_Texture), (D3D11_UNORDERED_ACCESS_VIEW_DESC*)& uavDesc, (ID3D11UnorderedAccessView * *)& m_UAV);
}
Texture2D::Texture2D(GraphicsDevice& device, size_t width, size_t height, unsigned int mipLevels, 
                     unsigned int arraySize, DXGI_FORMAT dxgiFormat, 
                     DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE usage, 
                     UINT bindFlags, UINT cpuAccessFlags,
                     UINT miscFlags, D3D11_SUBRESOURCE_DATA* initialData)
                     : m_Width(width), m_Height(height), m_MipLevels(mipLevels),
                       m_ArraySize(arraySize), m_DXGIFormat(dxgiFormat),
                       m_SampleDesc(sampleDesc), m_Usage(usage),
                       m_BindFlags(bindFlags), m_CPUAccessFlags(cpuAccessFlags),
                       m_MiscFlags(miscFlags), m_UAV(nullptr), m_SRV(nullptr)
{
    D3D11_TEXTURE2D_DESC desc;
    FillDesc(desc);

    D3D_HR_OP(device.GetD3D11Device()->CreateTexture2D(&desc, initialData, (ID3D11Texture2D**)(&m_Texture)));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Format = GetFormatSRV(m_DXGIFormat, m_BindFlags);
    srvDesc.ViewDimension = m_SampleDesc.Count > 1 ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    uavDesc.Texture2D.MipSlice = 0;
    uavDesc.Format = GetFormatSRV(m_DXGIFormat, m_BindFlags);
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;

    if ((bindFlags & D3D11_BIND_SHADER_RESOURCE ))
        device.GetD3D11Device()->CreateShaderResourceView((ID3D11Texture2D*)(m_Texture), (D3D11_SHADER_RESOURCE_VIEW_DESC*)&srvDesc, (ID3D11ShaderResourceView**)&m_SRV);
    if (!(bindFlags & D3D11_BIND_DEPTH_STENCIL) && 
        (bindFlags & D3D11_BIND_UNORDERED_ACCESS ))
        device.GetD3D11Device()->CreateUnorderedAccessView((ID3D11Texture2D*)(m_Texture), (D3D11_UNORDERED_ACCESS_VIEW_DESC*)& uavDesc, (ID3D11UnorderedAccessView * *)& m_UAV);
}

ID3D11ShaderResourceView* Texture2D::GetSRV() const
{
    return m_SRV;
}

ID3D11UnorderedAccessView* Texture2D::GetUAV() const
{
    return m_UAV;
}


Texture2D::~Texture2D()
{
}

void Texture2D::Resize(GraphicsDevice& device, size_t width, size_t height, MultisampleType msType)
{
    if (m_Width != width || m_Height != height || ((GetSamplesCount() != (int)msType)  && msType))
    {
        m_Width = width;
        m_Height = height;
        ReleaseGPUData();
        *this = Texture2D(device, m_Width, m_Height, 
                            m_MipLevels, m_ArraySize, 
                            m_DXGIFormat, msType ? GetSampleDesc(device, m_DXGIFormat, msType) : m_SampleDesc, 
                            m_Usage, m_BindFlags, 
                            m_CPUAccessFlags, m_MiscFlags, nullptr);
    }
}

size_t Texture2D::GetWidth() const
{
    return m_Width;
}
size_t Texture2D::GetHeight() const
{
    return m_Height;
}
size_t Texture2D::GetConsumedVideoMemorySize()
{
    return m_Width*m_Height*m_ArraySize*GetBytesPerPixelForDXGIFormat(m_DXGIFormat);
}

ID3D11Texture2D* Texture2D::GetD3D11Texture2D() const
{
    return (ID3D11Texture2D*)m_Texture;
}

void Texture2D::FillDesc(D3D11_TEXTURE2D_DESC& desc)
{
    desc.Width = m_Width;
    desc.Height = m_Height;
    desc.MipLevels = m_MipLevels;
    desc.ArraySize = m_ArraySize;
    desc.Format = m_DXGIFormat;
    desc.SampleDesc = m_SampleDesc;
    desc.Usage = m_Usage;
    desc.BindFlags = m_BindFlags;
    desc.CPUAccessFlags = m_CPUAccessFlags;
    desc.MiscFlags = m_MiscFlags;
}
size_t Texture2D::GetBytesPerPixelForDXGIFormat(DXGI_FORMAT format)
{
    if (format == 0)
        return 0;
    if (format >= DXGI_FORMAT_R32G32B32A32_TYPELESS && format <= DXGI_FORMAT_R32G32B32A32_SINT)
        return 16;
    if (format >= DXGI_FORMAT_R32G32B32_TYPELESS && format <= DXGI_FORMAT_R32G32B32_SINT)
        return 12;
    if (format >= DXGI_FORMAT_R16G16B16A16_TYPELESS && format <= DXGI_FORMAT_X32_TYPELESS_G8X24_UINT)
        return 8;
    if (format >= DXGI_FORMAT_R10G10B10A2_TYPELESS && format <= DXGI_FORMAT_X24_TYPELESS_G8_UINT)
        return 4;
    if (format >= DXGI_FORMAT_R8G8_TYPELESS && format <= DXGI_FORMAT_R16_SINT)
        return 2;
    if (format >= DXGI_FORMAT_R8_TYPELESS && format <= DXGI_FORMAT_A8_UNORM)
        return 1;
    popAssert(false); // if you got assert here it means u need to add some format support for this function
    return 0;
}

bool Texture2D::IsValid() const
{
    return m_Texture != nullptr;
}

void Texture2D::ReleaseGPUData() 
{
    int rc = 0;
    if (m_SRV)
        rc = m_SRV->Release();
    m_SRV = nullptr;
    if (m_UAV)
        rc = m_UAV->Release();
    m_UAV = nullptr;
    
    if (m_Texture)
        rc = m_Texture->Release();//we ve got auto release, right? wtf? errors got without this line, to investigate
    m_Texture = nullptr;
    
}

Texture2D Texture2DHelper::CreateCommonTexture(GraphicsDevice& device, size_t width, size_t height,
                                               unsigned int mipLevels, DXGI_FORMAT format,
                                               MultisampleType multisample, UINT bindFlags, 
                                               D3D11_SUBRESOURCE_DATA* initialData)
{
    return Texture2D(device, width, height, mipLevels, 1, format, GetSampleDesc(device, format, multisample), D3D11_USAGE_DEFAULT, bindFlags, 0, 0, initialData);
}