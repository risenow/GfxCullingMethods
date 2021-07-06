#include "basicvsconstants.h"

void VSEntry(in float4 pos : POSITION0, 
             in float4 icolor : COLOR0,
             out float4 opos : POSITION0,
             out float4 ocolor : NORMAL0
    )
{
    ocolor = icolor;
    float4 wpos = mul(model, pos);
    float4 vpos = mul(view, wpos);
    opos = vpos;//mul(projection, vpos);
}