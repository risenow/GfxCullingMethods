#pragma once
#include "Window.h"
#include "GraphicsDevice.h"
#include "multisampleutils.h"
#include "GraphicsSurface.h"

class GraphicsSwapChain
{
public:
    GraphicsSwapChain();
    GraphicsSwapChain(GraphicsDevice& device, const Window& window, MultisampleType multisample = MULTISAMPLE_TYPE_NONE);

    ~GraphicsSwapChain();

    void Present();

    ID3D11Texture2D* GetBackBufferTexture();
    ColorSurface GetBackBufferSurface();

    IDXGISwapChain* GetDXGISwapChain();

    bool IsValid(const Window& window, MultisampleType currentlySelectedMultisampleType);

    void Validate(GraphicsDevice& device, const Window& window, MultisampleType currentlySelectedMultisampleType);

    void InitializeBackBufferSurface(GraphicsDevice& device);
    void SetFullcreenState(bool state);

    void ReleaseGPUData();
    //ID3D11RenderTargetView* GetBackBufferRenderTargetView();
private:
    void FillSwapChainDesc(const Window& window, DXGI_SWAP_CHAIN_DESC& desc, const DXGI_SAMPLE_DESC& sampleDesc);
    void UpdateDisplayModesList();
    void UpdateDisplayModesList(IDXGIOutput* output);

    IDXGISwapChain* m_SwapChain;
    //ID3D11Texture2D* m_BackBufferTexture;
    Texture2D m_BackBufferTexture;
    ColorSurface m_BackBufferSurface;
    //ID3D11RenderTargetView* m_BackBufferRenderTargetView;
    DXGI_FORMAT m_Format;
    std::vector<DXGI_MODE_DESC> m_DisplayModeDescsList;
    //size_t m_Width;
    //size_t m_Height;

    bool m_FullsreenState;
};