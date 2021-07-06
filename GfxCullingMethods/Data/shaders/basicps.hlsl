SamplerState Sampler;
Texture2D diffuseMap: register(t0);

float4 PSEntry(
//#ifdef INSTANCED
//			   in uint instanceID : SV_InstanceID,
//#endif
			   in float4 pos : SV_POSITION, 
			   in float4 normal : NORMAL1, 
			   in float2 tc : TEXCOORD1) : SV_TARGET
{
	return diffuseMap.Sample(Sampler, tc);;
}