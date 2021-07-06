#pragma once
#include "ShaderStorage.h"

class BasicVertexShaderStorage : public ShaderStorage
{
public:
    static BasicVertexShaderStorage& GetInstance()
    {
        static BasicVertexShaderStorage inst;
        return inst;
    }
    BasicVertexShaderStorage() {
        m_ShaderFile = L"Data/shaders/basicvs.hlsl";
        BasicVertexShaderStorage::NORMALS_ENABLED = 1 << 0;
        BasicVertexShaderStorage::TEXCOORDS_ENABLED = 1 << 1;
        BasicVertexShaderStorage::INSTANCED = 1 << 2;
        m_MacroSet = { GraphicsShaderMacro("NORMALS_ENABLED", "1"), GraphicsShaderMacro("TEXCOORDS_ENABLED", "1"), GraphicsShaderMacro("INSTANCED", "1") };
    }
    void Load(GraphicsDevice& device)
    {
        ShaderStorage::Load(device, GraphicsShaderType_Vertex);
    }

    static size_t NORMALS_ENABLED;
    static size_t TEXCOORDS_ENABLED;
    static size_t INSTANCED;

};