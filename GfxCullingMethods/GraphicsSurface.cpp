#include "GraphicsSurface.h"
#include "logicsafety.h"
#include "dxlogicsafety.h"


template<class T>
void CreateView(GraphicsDevice& device, Texture2D texture, T** view) {}

template<>
void CreateView(GraphicsDevice& device, Texture2D texture, ID3D11RenderTargetView** view)
{
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
    rtvDesc.Format = texture.GetFormat();
    rtvDesc.ViewDimension = texture.GetSamplesCount() > 1 ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

    D3D_HR_OP(device.GetD3D11Device()->CreateRenderTargetView(texture.GetD3D11Texture2D(), &rtvDesc, view));
}

DXGI_FORMAT GetFormatDSV(DXGI_FORMAT format)
{
    if (format == DXGI_FORMAT_R24G8_TYPELESS)
        return DXGI_FORMAT_D24_UNORM_S8_UINT;
    return format;
}

template<>
void CreateView(GraphicsDevice& device, Texture2D texture, ID3D11DepthStencilView** view)
{
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
    dsvDesc.Format = GetFormatDSV(texture.GetFormat());
    dsvDesc.ViewDimension = texture.GetSamplesCount() > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;

    D3D_HR_OP(device.GetD3D11Device()->CreateDepthStencilView(texture.GetD3D11Texture2D(), &dsvDesc, view));
}


template<class T>
GraphicsSurface<T>::GraphicsSurface() {}

template<class T>
GraphicsSurface<T>::GraphicsSurface(GraphicsDevice& device, Texture2D texture)
                                : m_Texture(texture), m_TextureIsOwned(false)
{
    CreateView(device, texture, (T**)(&m_View));
}
template<class T>
GraphicsSurface<T>::GraphicsSurface(GraphicsDevice& device, size_t width, size_t height,
    unsigned int mipLevels, unsigned int arraySize, DXGI_FORMAT dxgiFormat,
    DXGI_SAMPLE_DESC sampleDesc, D3D11_USAGE usage, UINT bindFlags,
    UINT cpuAccessFlags, UINT miscFlags)
    : m_Texture(device, width, height, mipLevels, arraySize, dxgiFormat, sampleDesc, usage, bindFlags, cpuAccessFlags, miscFlags), m_TextureIsOwned(false)
{
    CreateView(device, m_Texture, (T**)(&m_View));
}

template<class T>
GraphicsSurface<T>::~GraphicsSurface()
{
}

template<class T>
void GraphicsSurface<T>::SetForeignTexture(GraphicsDevice& device, const Texture2D& texture)
{
    ReleaseGPUData();
    m_Texture = texture;
    CreateView(device, texture, (T**)(&m_View));
    m_TextureIsOwned = false;
}

#define RENDERDOC
template<class T>
void GraphicsSurface<T>::Resize(GraphicsDevice& device, size_t width, size_t height, MultisampleType msType)
{
    popAssert(m_TextureIsOwned);
    popAssert(m_Texture.IsValid());
    popAssert(m_View);

    if (m_Texture.GetWidth() != width || m_Texture.GetWidth() != height || m_Texture.GetSamplesCount() != (int)msType)
    {
        m_View->Release();
        T* view = m_View; //need this additional step cause clang optimization is stupidly aggresive here
        m_Texture.Resize(device, width, height, msType);


        CreateView(device, m_Texture, (T**)(&view));
#ifndef RENDERDOC
        popAssert(view != m_View);
#endif
        m_View = view;
    }
}

template<class T>
size_t GraphicsSurface<T>::GetWidth() const
{
    return m_Texture.GetWidth();
}
template<class T>
size_t GraphicsSurface<T>::GetHeight() const
{
    return m_Texture.GetHeight();
}

template<class T>
void GraphicsSurface<T>::ReleaseGPUData()
{
    int rc = m_View->Release();
    m_View = nullptr;
    if (m_TextureIsOwned)
        m_Texture.ReleaseGPUData();
}

template class GraphicsSurface<ID3D11RenderTargetView>;
template class GraphicsSurface<ID3D11DepthStencilView>;