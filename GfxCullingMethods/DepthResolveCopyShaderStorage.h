#pragma once
#include "ShaderStorage.h"

class DepthResolveCopyShaderStorage : public ShaderStorage
{
public:
    static DepthResolveCopyShaderStorage& GetInstance()
    {
        static DepthResolveCopyShaderStorage inst;
        return inst;
    }
    DepthResolveCopyShaderStorage() {
        m_ShaderFile = L"Data/shaders/copydepthcs.hlsl";
        DepthResolveCopyShaderStorage::RESOLVE_ENABLED = 1 << 0;

        m_MacroSet = { GraphicsShaderMacro("RESOLVE", "1") };
    }
    void Load(GraphicsDevice& device)
    {
        ShaderStorage::Load(device, GraphicsShaderType_Compute);
    }

    static size_t RESOLVE_ENABLED;

};