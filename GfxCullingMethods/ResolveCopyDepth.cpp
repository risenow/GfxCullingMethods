#include "ResolveCopyDepth.h"

void ResolveCopyDepth(GraphicsDevice& device, Texture2D& dest, const Texture2D& src)
{
    GraphicsShader copyShader = DepthResolveCopyShaderStorage::GetInstance().GetShader(src.GetSamplesCount() > 1 ? DepthResolveCopyShaderStorage::RESOLVE_ENABLED : 0);
    copyShader.Bind(device);

    ID3D11ShaderResourceView* srv = src.GetSRV();
    ID3D11UnorderedAccessView* uav = dest.GetUAV();

    device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &srv);
    device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

    device.GetD3D11DeviceContext()->Dispatch(ceil((float)src.GetWidth() / 32.0f), ceil((float)src.GetHeight() / 32.0f), 1);

    srv = nullptr;
    uav = nullptr;
    device.GetD3D11DeviceContext()->CSSetShaderResources(0, 1, &srv);
    device.GetD3D11DeviceContext()->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
    device.GetD3D11DeviceContext()->CSSetShader(nullptr, nullptr, 0);
}