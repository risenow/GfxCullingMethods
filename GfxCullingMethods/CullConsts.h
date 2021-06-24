#ifndef CULL_CONSTANTS
#define CULL_CONSTANTS

#include "ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE CullConsts
{
    DECLARE_UINT drawCount;
    DECLARE_UINT padding1;
    DECLARE_UINT padding2;
    DECLARE_UINT padding3;

    DECLARE_FLOAT4 frLeft;
    DECLARE_FLOAT4 frRight;
    DECLARE_FLOAT4 frTop;
    DECLARE_FLOAT4 frBottom;

    DECLARE_FLOAT4 rightDir;
    DECLARE_FLOAT4 topDir;
    DECLARE_FLOAT4 camPos;
    DECLARE_FLOAT4 padding4;
    DECLARE_FLOAT4X4 viewProj;
    DECLARE_FLOAT4X4 view;
    DECLARE_FLOAT4X4 proj;
};

#endif