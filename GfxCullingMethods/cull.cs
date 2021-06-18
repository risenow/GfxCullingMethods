#include "CullConsts.h"

struct Instance
{
	uint batchIndex;
	float4x4 transform;
	float4 sbb;
};

StructuredBuffer<Instance> refInstances : register(t0);
RWStructuredBuffer<BufType> renderInstances : register(u0);
RWByteAddressBuffer args : register(u1);

float TestPlane(float4 plane, float3 p)
{
	return plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w;
}

bool CullSphere(float4 sphere)
{
	float d = TestPlane(frLeft, sphere.xyz);
	d = max(d, TestPlane(frRight, sphere.xyz);
	d = max(d, TestPlane(frTop, sphere.xyz);
	d = max(d, TestPlane(frBottom, sphere.xyz);
	
	return -d > sphere.w;
}

#define ARGS_SZ 16

void IncInstanceCount(uint batchIndex, out uint count, out uint start)
{
	args.InterlockedAdd(ARGS_SZ*batchIndex + 4, 1, count);
	start = args.Load(ARGS_SZ*batchIndex + 12);
}


[numthreads(256,1,1)]
void CSEntry(uint3 id : SV_DispatchThreadID)
{
	uint globalID = id.x;
	
	if (globalID < drawCount)
	{
		Instance inst = refInstances[globalID];
		uint batchIndex = inst.batchIndex;
		float4 sphere = inst.sbb;
		float4 spherePos = mul(inst.transform, float4(sphere, 1.0));
		sphere = float4(spherePos.x, spherePos.y, spherePos.z, sphere.w);
		
		if (!CullSphere(sphere))
		{
			uint count = 0;
			uint start = 0;
			
			IncInstanceCount(batchIndex, count, start);
			renderInstances[start+count] = inst;
		}
	}
}