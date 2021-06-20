#ifndef BASIC_VS_CONSTANTS
#define BASIC_VS_CONSTANTS

#include "ConstantsPreprocessing.h"

DECLARE_CONSTANT_STRUCTURE BasicVSConsts
{
    DECLARE_FLOAT4X4 model;
    DECLARE_FLOAT4X4 projection;
    DECLARE_FLOAT4X4 view;
    DECLARE_UINT instancesOffset;
    DECLARE_UINT padding1;
    DECLARE_UINT padding2;
    DECLARE_UINT padding3;
    DECLARE_UINT padding4;
    DECLARE_UINT padding5;
    DECLARE_UINT padding6;
    DECLARE_UINT padding7;

};

#endif
