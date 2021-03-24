#include "basicvsconstants.h"

struct Vertex
{
    float4 vpos : POSITION0;
    float4 color : NORMAL0;
};
struct GSVertex
{
    float4 opos : SV_POSITION;
    float4 color : NORMAL0;
};

[maxvertexcount(5)]
void GSEntry(line Vertex gin[2], inout TriangleStream<GSVertex> triStream)
{
    const float width = 0.5f;
    float3 temp1 = (gin[0].vpos.xyz - gin[1].vpos.xyz);
    float3 temp2 = gin[0].vpos.xyz;
    float3 scaledSideVec = normalize(cross(temp1, temp2)) * width;//float3(0.1, 0.1, 0.1);//
    
    float4 v1 = mul(projection, float4(gin[0].vpos.xyz + scaledSideVec, 1.0));
    float4 v2 = mul(projection, float4(gin[0].vpos.xyz - scaledSideVec, 1.0));
    float4 v3 = mul(projection, float4(gin[1].vpos.xyz - scaledSideVec, 1.0));
    float4 v4 = mul(projection, float4(gin[1].vpos.xyz + scaledSideVec, 1.0));

    triStream.RestartStrip();

    GSVertex temp;
    temp.opos = v1;
    temp.color = gin[0].color;
    triStream.Append(temp);
    temp.opos = v2;
    temp.color = gin[0].color;
    triStream.Append(temp);
    temp.opos = v3;
    temp.color = gin[1].color;
    triStream.Append(temp);
    temp.opos = v4;
    temp.color = gin[1].color;
    triStream.Append(temp);
    temp.opos = v1;
    temp.color = gin[0].color;
    triStream.Append(temp);
}