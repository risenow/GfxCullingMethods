#include "basicvsconstants.h"

void VSEntry(in float4 pos : POSITION0, 
             in float4 inormal : NORMAL0,
             in float2 itc : TEXCOORD0,
             out float4 opos : SV_POSITION,
             out float4 onormal : NORMAL0,
             out float2 otc : TEXCOORD0
    )
{
    otc = itc;
    onormal = inormal;
    float4 wpos = mul(model, pos);
    float4 vpos = mul(view, wpos);
    opos = mul(projection, vpos);
    //opos = pos;
}