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
Texture2D hiZ : register(t1);

RWStructuredBuffer<Instance> renderInstances : register(u0);
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

	return !(-d < sphere.w);
}
float4 summAffine(float4 v1, float4 v2)
{
	return float4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, 1.0f);
}
void ProjectSphere(float4 sphere, out float4 aabb2D)
{
	float4 spherePos = float4(sphere.x, sphere.y, sphere.z, 1.0f);

	float4 temp;
	float4 sphereRight = mul(viewProj, summAffine(spherePos, rightDir * sphere.w));
	sphereRight /= sphereRight.w;

	float4 sphereLeft = mul(viewProj, summAffine(spherePos, -rightDir * sphere.w));
	sphereLeft /= sphereLeft.w;

	float4 sphereTop = mul(viewProj, summAffine(spherePos, topDir * sphere.w));
	sphereTop /= sphereTop.w;

	float4 sphereBot = mul(viewProj, summAffine(spherePos, -topDir * sphere.w));
	sphereBot /= sphereBot.w;

	aabb2D.x = sphereLeft.x;
	aabb2D.y = sphereRight.x;
	aabb2D.z = sphereBot.y;
	aabb2D.w = sphereTop.y;
}

float CalcSphereMinDepth(float4 sphere)
{
	float3 sphereMinDepthPos = sphere.xyz + normalize(camPos.xyz-sphere.xyz)*sphere.w;
	float4 minDepthPosProj = mul(viewProj, float4(sphereMinDepthPos.x, sphereMinDepthPos.y, sphereMinDepthPos.z, 1.0f));
	minDepthPosProj /= minDepthPosProj.w;

	return minDepthPosProj.z;
}

float2 ndcToFlippedUV(float2 ndc)
{
	float2 uv = (ndc + float2(1.0f, 1.0f)) * 0.5f;

	return  float2(uv.x, 1.0f - uv.y);
}

bool OcclusionCullSphere(float4 sphere)
{
	if (length(sphere.xyz - camPos.xyz) <= sphere.w)
		return false;

	uint width;
	uint height;
	uint lvlNum;
	hiZ.GetDimensions(0, width, height, lvlNum);

	float4 aabb2D;
	ProjectSphere(sphere, aabb2D);
	
	float aabbWidth = max(((aabb2D.y - aabb2D.x) * width * 0.5f), 1.001f);
	float aabbHeight = max(((aabb2D.w - aabb2D.z) * height * 0.5f), 1.001f);

	uint level = min((uint)(ceil(log2(max(max(aabbWidth, aabbHeight), 1.0f)))), lvlNum - 1);

	hiZ.GetDimensions(level, width, height, lvlNum);

	float texelXSz = 1.0f / width;
	float texelYSz = 1.0f / height;

	float minSphereDepth = CalcSphereMinDepth(sphere);

	float2 flippedUV = ndcToFlippedUV(float2(aabb2D.x, aabb2D.w));
	uint3 topLeft = uint3((uint)(flippedUV.x*width), (uint)(flippedUV.y * height), level);
	float s1 = hiZ.Load(topLeft);
	float s2 = hiZ.Load(topLeft + uint3(1, 0, 0));
	float s3 = hiZ.Load(topLeft + uint3(0, 1, 0));
	float s4 = hiZ.Load(topLeft + uint3(1, 1, 0));

	bool outOfBounds = flippedUV.x > 1.0f || flippedUV.y > 1.0f || flippedUV.x < 0.0f-texelXSz || flippedUV.y < 0.0f - texelYSz;

	return (!(minSphereDepth < s1 || minSphereDepth < s2 || minSphereDepth < s3 || minSphereDepth < s4)) && !outOfBounds;
}

#define ARGS_SZ 16

void IncInstanceCount(uint batchIndex, out uint count, out uint start)
{	
	args.InterlockedAdd(ARGS_SZ*batchIndex + 4, 1u, count);
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

		float4 spherePos = mul(inst.transform, float4(sphere.x, sphere.y, sphere.z, 1.0));
		sphere = float4(spherePos.x, spherePos.y, spherePos.z, sphere.w);
		
		if (
			!CullSphere(sphere) && 
			!OcclusionCullSphere(sphere)
			)
		{
			uint count;
			uint start;
			
			IncInstanceCount(batchIndex, count, start);
			
			renderInstances[start+count] = inst;

		}
	}
}