#pragma once
#include <vector>
#include "GraphicsShader.h"

class MeshShader
{
public:
    static MeshShader& GetInstance()
    {
        MeshShader shader;
        return shader;
    }

private:

};