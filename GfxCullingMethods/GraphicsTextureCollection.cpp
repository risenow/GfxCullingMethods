#include "GraphicsTextureCollection.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"
#include "WICTextureLoader11.h"
#include "DDSTextureLoader11.h"
#include <algorithm>
#include <string>

bool IsFileOfSupported2DFormat(const std::string& file, std::string& format)
{
    if (!file.size())
        return false;
    format.reserve(6);
    for (int i = file.size() - 1; (i >= 0) && (file[i] != '.'); i--)
        format.push_back(file[i]);
    std::reverse(format.begin(), format.end());

    bool supported = false;
    if (format == "jpg" || format == "JPG" || format == "png" || format == "bmp")
        supported = true;

    return supported;
}

GraphicsTextureCollection::GraphicsTextureCollection(GraphicsDevice& device)
{
    InitializeWhiteTexture(device);
    InitializeBlackTexture(device);
    InitializeBlueTexture(device);
}
GraphicsTextureCollection::~GraphicsTextureCollection() {}

auto GraphicsTextureCollection::Add(GraphicsDevice& device, const std::string& file, bool isNormalMap, DXGI_FORMAT format)
{
    std::string sformat;
    if (IsFileOfSupported2DFormat(file, sformat))
    {
        ID3D11Resource* res;
        ID3D11ShaderResourceView* srv;
        HRESULT hr = DirectX::CreateWICTextureFromFile(device.GetD3D11Device(), strtowstr_fast(file).c_str(), (ID3D11Resource**)&res, &srv);

        if (SUCCEEDED(hr))
        {
            auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, srv) });
            return i.first;
        }

        return end();
    }

    if (sformat == "dds")
    {
        ID3D11Resource* res;
        ID3D11ShaderResourceView* srv;
        HRESULT hr = DirectX::CreateDDSTextureFromFile(
            device.GetD3D11Device(),
            strtowstr_fast(file).c_str(),
            (ID3D11Resource**)&res,
            &srv);

        if (SUCCEEDED(hr))
        {
            auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, srv) });
            return i.first;
        }
        return end();
    }

    if (sformat == "tga")
    {
        ID3D11Resource* res = nullptr;
        DirectX::ScratchImage scrImage;
        DirectX::TexMetadata texMetaData;
        DirectX::ScratchImage mipChain;
        DirectX::TexMetadata mipChainMetaData;

        HRESULT hr = (DirectX::LoadFromTGAFile(strtowstr_fast(file).c_str(), &texMetaData, scrImage));

        int levels = 0; //mb use lower levels num for normal maps

        HRESULT hr2 = -1;
        if (scrImage.GetImageCount())
        {
            DirectX::GenerateMipMaps(scrImage.GetImages()[0], DirectX::TEX_FILTER_DEFAULT, levels, mipChain, false);
            hr2 = (DirectX::CreateTextureEx(device.GetD3D11Device(), mipChain.GetImages(), mipChain.GetImageCount(), mipChain.GetMetadata(), D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, !isNormalMap, &res));
        }

        if (SUCCEEDED(hr) && SUCCEEDED(hr2))
        {
            auto i = insert({ file, std::make_shared<Texture2D>(device, (ID3D11Texture2D*)res, nullptr) });
            return i.first;
        }
    }

    return end();
}
Texture2D* GraphicsTextureCollection::GetBlackTexture()
{
    return &m_BlackTexture;
}
Texture2D* GraphicsTextureCollection::GetBlueTexture()
{
    return &m_BlueTexture;
}
Texture2D* GraphicsTextureCollection::GetWhiteTexture()
{
    return &m_WhiteTexture;
}
Texture2D GraphicsTextureCollection::InitializeMonotoneTexture(GraphicsDevice& device, uint8_t color[4])
{
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = (void*)color;
    data.SysMemPitch = sizeof(uint8_t) * 4;
    data.SysMemSlicePitch = 0;
    return Texture2D(device, 1, 1, 1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, GetDefaultSampleDesc(), D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, &data);
}
void GraphicsTextureCollection::InitializeBlackTexture(GraphicsDevice& device)
{
    uint8_t p[4] = { 0, 0, 0, 255 };
    m_BlackTexture = InitializeMonotoneTexture(device, p);
}
void GraphicsTextureCollection::InitializeBlueTexture(GraphicsDevice& device)
{
    uint8_t p[4] = { 0, 0, 255, 255 };
    m_BlueTexture = InitializeMonotoneTexture(device, p);
}
void GraphicsTextureCollection::InitializeWhiteTexture(GraphicsDevice& device)
{
    uint8_t p[4] = { 255, 255, 255, 255 };
    m_WhiteTexture = InitializeMonotoneTexture(device, p);
}

Texture2D* GraphicsTextureCollection::RequestTexture(GraphicsDevice& device, const std::string& file, bool isNormalMap) //mb return shared ptr?
{
    auto it = find(file);
    if (it == end())
    {
        auto it_ = Add(device, file, isNormalMap);
        if (it_ != end())
            return (*it_).second.get();
    }
    else
        return it->second.get();
    return nullptr;
}
