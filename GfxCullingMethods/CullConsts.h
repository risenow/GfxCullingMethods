#ifndef CULL_CONSTANTS
#define CULL_CONSTANTS

#include "ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE CullConsts
{
    DECLARE_UINT drawCount;
    DECLARE_FLOAT4 frLeft;
    DECLARE_FLOAT4 frRight;
    DECLARE_FLOAT4 frTop;
    DECLARE_FLOAT4 frBottom;

};

#endif