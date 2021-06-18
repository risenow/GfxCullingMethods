#pragma once
#include "GraphicsShader.h"

class ShaderStorage
{
public:
    void Load(GraphicsDevice& device, GraphicsShaderType type)
    {
        std::vector<ExtendedShaderVariation> variations = GetAllPermutations(m_MacroSet);
        for (ExtendedShaderVariation& var : variations)
        {
            m_Shaders.push_back(GraphicsShader::FromFile(device, type, m_ShaderFile.c_str(), var.m_ShaderVariation));
        }
    }
    GraphicsShader& GetShader(size_t var)
    {
        return m_Shaders[var];
    }

protected:
    std::vector<GraphicsShaderMacro> m_MacroSet;
    std::wstring m_ShaderFile;
    std::vector<GraphicsShader> m_Shaders;
};