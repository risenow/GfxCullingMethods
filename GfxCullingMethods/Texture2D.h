#pragma once
#include <d3d11.h>
#include "GraphicsDevice.h"
#include "MultisampleType.h"
#include <string>

class Texture2D
{
public:
    Texture2D();
    Texture2D(GraphicsDevice& device, ID3D11Texture2D* texture, ID3D11ShaderResourceView* srv = nullptr);
    Texture2D(GraphicsDevice& device, size_t width, size_t height, 
              unsigned int mipLevels, unsigned int arraySize, DXGI_FORMAT dxgiFormat, 
              DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE usage, UINT bindFlags,
              UINT cpuAccessFlags, UINT miscFlags, D3D11_SUBRESOURCE_DATA* initialData = nullptr);
    //Texture2D(Texture2D& other);
    ~Texture2D();
    void Resize(GraphicsDevice& device, size_t width, size_t height, MultisampleType msType);

    size_t GetWidth() const;
    size_t GetHeight() const;
    size_t GetConsumedVideoMemorySize();
    size_t GetSamplesCount() const { return m_SampleDesc.Count; }
    DXGI_FORMAT GetFormat() const { return m_DXGIFormat; }
    size_t GetMipsCount() const { return m_MipLevels; }
    DXGI_SAMPLE_DESC GetSampleDescr() const { return m_SampleDesc; }

    ID3D11Texture2D* GetD3D11Texture2D() const;

    ID3D11ShaderResourceView* GetSRV() const;
    ID3D11UnorderedAccessView* GetUAV() const;

    bool IsValid() const;

    void ReleaseGPUData();

    static Texture2D LoadDeprecated(GraphicsDevice& device, const std::string& file); //loads only png
private:
    void FillDesc(D3D11_TEXTURE2D_DESC& desc);
    size_t GetBytesPerPixelForDXGIFormat(DXGI_FORMAT format);

    size_t m_Width;
    size_t m_Height;
    unsigned int m_MipLevels;
    unsigned int m_ArraySize;
    DXGI_FORMAT m_DXGIFormat;
    DXGI_SAMPLE_DESC m_SampleDesc;
    D3D11_USAGE m_Usage;
    UINT m_BindFlags;
    UINT m_CPUAccessFlags;
    UINT m_MiscFlags;

    ID3D11Texture2D* m_Texture;
    ID3D11ShaderResourceView* m_SRV;
    ID3D11UnorderedAccessView* m_UAV;
};

class Texture2DHelper 
{
public:
    static Texture2D CreateCommonTexture(GraphicsDevice& device, size_t width, size_t height, 
                                         unsigned int mipLevels, DXGI_FORMAT format, MultisampleType multisample,
                                         UINT bindFlags, D3D11_SUBRESOURCE_DATA* initialData = nullptr);
};