#include "basicvsconstants.h"
struct Instance
{
    uint batchIndex;
    float4x4 transform;
    float4 sbb;
};
StructuredBuffer<Instance> instances : register(t0);

void VSEntry(
    in float4 pos : POSITION0
#ifdef NORMALS_ENABLED
             ,in float4 inormal : NORMAL0
#endif
#ifdef TEXCOORDS_ENABLED
             ,in float2 itc : TEXCOORD0
#endif
#ifdef INSTANCED
    ,uint instanceID : SV_InstanceID
#endif
             ,out float4 opos : SV_POSITION
#ifdef NORMALS_ENABLED
             ,out float4 onormal : NORMAL1
#endif
#ifdef TEXCOORDS_ENABLED
             ,out float2 otc : TEXCOORD1
#endif

    )
{
#ifdef TEXCOORDS_ENABLED
    otc = itc;
#endif
#ifdef NORMALS_ENABLED
    onormal = inormal;
#endif
    float4 wpos = mul(model, pos);
#ifdef INSTANCED
    wpos = mul(instances[instanceID + instancesOffset].transform, pos);
#endif
    float4 vpos = mul(view, wpos);
    opos = mul(projection, vpos);
    //opos = pos;
}