#pragma once
#include "ConstantsPreprocessing.h"

struct DrawInstancedIndirectArgs
{
    DECLARE_UINT VertexCountPerInstance;
    DECLARE_UINT InstanceCount;
    DECLARE_UINT StartVertexLocation;
    DECLARE_UINT StartInstanceLocation;

};