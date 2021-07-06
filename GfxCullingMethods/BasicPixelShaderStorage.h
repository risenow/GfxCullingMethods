#pragma once
#include "ShaderStorage.h"

class BasicPixelShaderStorage : public ShaderStorage
{
public:
    static BasicPixelShaderStorage& GetInstance()
    {
        static BasicPixelShaderStorage inst;
        return inst;
    }
    BasicPixelShaderStorage() {
        m_ShaderFile = L"Data/shaders/basicps.hlsl";
        BasicPixelShaderStorage::NORMALS_ENABLED = 1 << 0;
        BasicPixelShaderStorage::TEXCOORDS_ENABLED = 1 << 1;
        BasicPixelShaderStorage::INSTANCED = 1 << 2;
        m_MacroSet = { GraphicsShaderMacro("NORMALS_ENABLED", "1"), GraphicsShaderMacro("TEXCOORDS_ENABLED", "1"),  GraphicsShaderMacro("INSTANCED", "1") };
    }
    void Load(GraphicsDevice& device)
    {
        ShaderStorage::Load(device, GraphicsShaderType_Pixel);
    }

    static size_t NORMALS_ENABLED;
    static size_t TEXCOORDS_ENABLED;
    static size_t INSTANCED;

};