#include "multisampleutils.h"
#include "logicsafety.h"
#include "dxlogicsafety.h"

unsigned int GetMultisampleQualityNum(GraphicsDevice& device, DXGI_FORMAT format, MultisampleType multisample)
{
    UINT qualityLevelsCount;
    D3D_HR_OP(device.GetD3D11Device()->CheckMultisampleQualityLevels(format, (UINT)multisample, &qualityLevelsCount));
    return qualityLevelsCount;
}
DXGI_SAMPLE_DESC GetSampleDesc(GraphicsDevice& device, DXGI_FORMAT format, MultisampleType multisample)
{
    DXGI_SAMPLE_DESC desc;
    if (multisample == MULTISAMPLE_TYPE_NONE)
    {
        desc.Count = 1;
        desc.Quality = 0;
    }
    else
    {
        unsigned int multisampleQualityLevelsCount = GetMultisampleQualityNum(device, format, multisample);
        popAssert(multisampleQualityLevelsCount);
        desc.Count = (UINT)multisample;
        desc.Quality = multisampleQualityLevelsCount - 1; //TODO: implement possibilty to select quality
    }
    return desc;
}
DXGI_SAMPLE_DESC GetDefaultSampleDesc()
{
    DXGI_SAMPLE_DESC desc;
    desc.Count = 1;
    desc.Quality = 0;

    return desc;
}