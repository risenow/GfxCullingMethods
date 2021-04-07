#include "GraphicsSwapchain.h"
#include "logicsafety.h"
#include "dxlogicsafety.h"
#include "multisampleutils.h"
#include <dxgi.h>
#include <thread>

GraphicsSwapChain::GraphicsSwapChain() {}
GraphicsSwapChain::GraphicsSwapChain(GraphicsDevice& device, const Window& window, MultisampleType multisample)
                                   : m_Format(DXGI_FORMAT_R8G8B8A8_UNORM), m_SwapChain(nullptr), m_FullsreenState(false)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    FillSwapChainDesc(window, swapChainDesc, GetSampleDesc(device, m_Format, multisample));

    IDXGIDevice* giDev;
    D3D_HR_OP(device.GetD3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)&giDev));
    IDXGIAdapter* giAdapter;
    D3D_HR_OP(giDev->GetAdapter(&giAdapter));
    IDXGIFactory* giFactory;
    D3D_HR_OP(giAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&giFactory));

    //D3D_HR_OP(giFactory->CreateSwapChain(device.GetD3D11Device(), &swapChainDesc, &m_SwapChain));
    HRESULT t = giFactory->CreateSwapChain(device.GetD3D11Device(), &swapChainDesc, &m_SwapChain);

    giFactory->MakeWindowAssociation(window.GetWindowHandle(), DXGI_MWA_NO_WINDOW_CHANGES);

    giDev->Release();
    giAdapter->Release();
    giFactory->Release();

    InitializeBackBufferSurface(device);
}

GraphicsSwapChain::~GraphicsSwapChain()
{
}

void GraphicsSwapChain::InitializeBackBufferSurface(GraphicsDevice& device)
{
    ID3D11Texture2D* backBuffer;
    D3D_HR_OP(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)(&backBuffer)));

    m_BackBufferTexture = Texture2D(device, backBuffer);
    //DEBUG_ONLY(m_BackBufferTexture.SetDebugName("Backbuffer Texture"));

    m_BackBufferSurface = ColorSurface(device, m_BackBufferTexture);   
}

IDXGISwapChain* GraphicsSwapChain::GetDXGISwapChain()
{
    return m_SwapChain;
}

void GraphicsSwapChain::Present()
{
    D3D_HR_OP(m_SwapChain->Present(0, 0));
}

bool GraphicsSwapChain::IsValid(const Window& window, MultisampleType currentlySelectedMultisampleType)
{
    if ((window.GetWindowMode() == WindowMode_FULLSCREEN) != m_FullsreenState)
        return false;
    DXGI_SWAP_CHAIN_DESC swapchainDesc;
    D3D_HR_OP(m_SwapChain->GetDesc(&swapchainDesc));
    DXGI_MODE_DESC bufferDesc = swapchainDesc.BufferDesc;
    if (window.GetWidth() != bufferDesc.Width || window.GetHeight() != bufferDesc.Height)// || swapchainDesc.SampleDesc.Count != (unsigned int)currentlySelectedMultisampleType)
        return false;
    return true;
}

void GraphicsSwapChain::Validate(GraphicsDevice& device, const Window& window, MultisampleType currentlySelectedMultisampleType)
{
    if (!IsValid(window, currentlySelectedMultisampleType))
    {
        m_BackBufferSurface.ReleaseGPUData();
        m_BackBufferTexture.ReleaseGPUData();

        bool fullscreen = window.GetWindowMode() == WindowMode_FULLSCREEN;
        m_FullsreenState = false;

        unsigned int newSwapchainWidth = window.GetWidth();
        unsigned int newSwapchainHeight = window.GetHeight();

        //IDXGIOutput* output;
        //D3D_HR_OP(m_SwapChain->GetContainingOutput(&output));
        //UpdateDisplayModesList(output);
        //output->Release();
        if (fullscreen)
        {
            IDXGIOutput* output;
            D3D_HR_OP(m_SwapChain->GetContainingOutput(&output));
            UpdateDisplayModesList(output);

            DXGI_MODE_DESC selectedDisplayMode = m_DisplayModeDescsList[m_DisplayModeDescsList.size() - 1];

            D3D_HR_OP(m_SwapChain->SetFullscreenState(true, output));
            D3D_HR_OP(m_SwapChain->ResizeTarget(&selectedDisplayMode));
            m_FullsreenState = true;

            newSwapchainWidth = selectedDisplayMode.Width;
            newSwapchainHeight = selectedDisplayMode.Height;

            output->Release();
        }
        //if (!m_FullsreenState)
        //    D3D_HR_OP(m_SwapChain->SetFullscreenState(false, nullptr));

        D3D_HR_OP(m_SwapChain->ResizeBuffers(1, newSwapchainWidth, newSwapchainHeight, m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));
        InitializeBackBufferSurface(device);
    }
}

//to change return type to Texture2D
ID3D11Texture2D* GraphicsSwapChain::GetBackBufferTexture()
{
    return nullptr;
}
ColorSurface GraphicsSwapChain::GetBackBufferSurface()
{
    return m_BackBufferSurface;
}

void GraphicsSwapChain::UpdateDisplayModesList()
{
    IDXGIOutput* output;
    D3D_HR_OP(m_SwapChain->GetContainingOutput(&output));
    UpdateDisplayModesList(output);
    output->Release();
}
void GraphicsSwapChain::UpdateDisplayModesList(IDXGIOutput* output)
{
    unsigned int modesNum = 0;
    D3D_HR_OP(output->GetDisplayModeList(m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH, &modesNum, nullptr));
    popAssert(modesNum);
    m_DisplayModeDescsList.resize(modesNum);
    D3D_HR_OP(output->GetDisplayModeList(m_Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH, &modesNum, m_DisplayModeDescsList.data()));
}
void GraphicsSwapChain::FillSwapChainDesc(const Window& window, DXGI_SWAP_CHAIN_DESC& desc, 
                                          const DXGI_SAMPLE_DESC& sampleDesc)
{
    desc.BufferDesc.Width = window.GetWidth();
    desc.BufferDesc.Height = window.GetHeight();
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = m_Format;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    desc.SampleDesc = sampleDesc;

    desc.BufferCount = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;// | DXGI_USAGE_UNORDERED_ACCESS;

    desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    desc.OutputWindow = window.GetWindowHandle();
    desc.Windowed = true;//(window.GetWindowMode() == WindowMode_WINDOWED);
}

void GraphicsSwapChain::SetFullcreenState(bool state)
{
    if (m_FullsreenState != state)
    {
        m_FullsreenState = state;
        //m_FullscreenStateChanged = true;
    }
}

void GraphicsSwapChain::ReleaseGPUData()
{
    m_SwapChain->Release();
    m_SwapChain = nullptr;

    m_BackBufferTexture.ReleaseGPUData();
}

