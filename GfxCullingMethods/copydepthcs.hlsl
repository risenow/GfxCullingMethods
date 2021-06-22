RWTexture2D<float> Dest : register(u0);

#ifdef RESOLVE
Texture2DMS<float> Src : register(t0);
#else 
Texture2D<float> Src : register(t0);
#endif

[numthreads(32, 32, 1)]
void CSEntry(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{

    int3 ta = int3(DTid.x, DTid.y, 0);
#ifdef RESOLVE
    uint2 dim;
    uint sampleCount;

    Src.GetDimensions(dim.x, dim.y, sampleCount);

    float v = Src.Load(ta, 0);
    for (uint i = 1; i < sampleCount; i++)
    {
        v = min(v, Src.Load(ta, i));
    }
#else
    float v = Src.Load(ta);
#endif
    
    Dest[ta.xy] = v;
}