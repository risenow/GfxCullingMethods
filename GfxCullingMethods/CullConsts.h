#ifndef CULL_CONSTANTS
#define CULL_CONSTANTS

#include "ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE CullConsts
{
    DECLARE_UINT drawCount;
    DECLARE_UINT padding1;
    DECLARE_UINT padding2;
    DECLARE_UINT padding3;
    DECLARE_UINT padding4;
    DECLARE_UINT padding5;
    DECLARE_UINT padding6;
    DECLARE_UINT padding7;
    DECLARE_FLOAT4 frLeft;
    DECLARE_FLOAT4 frRight;
    DECLARE_FLOAT4 frTop;
    DECLARE_FLOAT4 frBottom;

};

#endif