SamplerState Sampler;
Texture2D diffuseMap: register(t0);

float4 PSEntry(in float4 pos : SV_POSITION, in float4 normal : NORMAL0, in float2 tc : TEXCOORD0) : SV_TARGET
{
	return diffuseMap.Sample(Sampler, tc);;
}