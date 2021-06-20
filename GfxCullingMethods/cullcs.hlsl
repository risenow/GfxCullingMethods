#include "CullConsts.h"

struct Instance
{
	uint batchIndex;
	float4x4 transform;
	float4 sbb;
};

struct DrawInstancedIndirectArgs
{
    uint VertexCountPerInstance;
    uint InstanceCount;
    uint StartVertexLocation;
    uint StartInstanceLocation;

};

StructuredBuffer<Instance> refInstances : register(t0);
globallycoherent RWStructuredBuffer<Instance> renderInstances : register(u0);
globallycoherent RWByteAddressBuffer args : register(u1);

float TestPlane(float4 plane, float3 p)
{
	return plane.x * p.x + plane.y * p.y + plane.z * p.z + plane.w;
}

bool CullSphere(float4 sphere)
{
	float d = TestPlane(frLeft, sphere.xyz);
	d = min(d, TestPlane(frRight, sphere.xyz));
	d = min(d, TestPlane(frTop, sphere.xyz));
	d = min(d, TestPlane(frBottom, sphere.xyz));
	
	return !(-d < sphere.w);//-d > sphere.w;
}

#define ARGS_SZ 16

void IncInstanceCount(uint batchIndex, out uint count, out uint start)
{
	//count=args.Load(ARGS_SZ*batchIndex + 4);
	
	//args.InterlockedAdd(ARGS_SZ*batchIndex + 4, 1u, count);
	//InterlockedAdd(args[ARGS_SZ*batchIndex + 4], 1);
	//count = 0;
	//start = args.Load(ARGS_SZ*batchIndex + 12);
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
		float4 spherePos = mul(inst.transform, float4(sphere.x, sphere.y, sphere.z, 1.0));
		sphere = float4(spherePos.x, spherePos.y, spherePos.z, sphere.w);
		
		if (!CullSphere(sphere))
		{
			uint count = 0;
			uint start = 0;
			
			//IncInstanceCount(batchIndex, count, start);
			args.InterlockedAdd(16*batchIndex + 4, 1, count);
			
			start = args.Load(16*batchIndex + 12);
			renderInstances[start+count] = inst;

		}
	}
}