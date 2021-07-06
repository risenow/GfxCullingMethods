SamplerState Sampler;
Texture2D diffuseMap: register(t0);

float4 PSEntry(in float4 pos : SV_POSITION, in float4 color :NORMAL0) : SV_TARGET
{
	return color;
}