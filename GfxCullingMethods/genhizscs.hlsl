//using mini engine mips generation idea
#include "GenHiZConsts.h"

RWTexture2D<float> OutMip1 : register(u0);
RWTexture2D<float> OutMip2 : register(u1);
RWTexture2D<float> OutMip3 : register(u2);
RWTexture2D<float> OutMip4 : register(u3);
Texture2D<float> SrcMip : register(t0);

groupshared float gsD[64];

void StoreDepth( uint Index, float d )
{
    gsD[Index] = d;
}

float LoadDepth( uint Index )
{
    return gsD[Index];
}


[numthreads( 8, 8, 1 )]
void CSEntry( uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID )
{
    int3 ta = int3(DTid.x*2, DTid.y*2, 0);
    float Src1 = SrcMip.Load(ta).r;
    Src1 = max(Src1, SrcMip.Load(ta + int3(1, 0, 0)).r);
    Src1 = max(Src1, SrcMip.Load(ta + int3(0, 1, 0)).r);
    Src1 = max(Src1, SrcMip.Load(ta + int3(1, 1, 0)).r);

    uint width = srcMipWidth;
    uint height = srcMipHeight;
    //uint lvls;
    //SrcMip.GetDimensions(0, width, height, lvls);

    bool handleOddWidth = ((width & 1) != 0) && DTid.x*2 == width - 3;
    bool handleOddHeight = ((height & 1) != 0) && DTid.y * 2 == height - 3;

    if (handleOddWidth)
    {
        Src1 = max(Src1, SrcMip.Load(ta + int3(2, 0, 0)).r);
        Src1 = max(Src1, SrcMip.Load(ta + int3(2, 1, 0)).r);
        
    }
    if (handleOddHeight)
    {
        Src1 = max(Src1, SrcMip.Load(ta + int3(0, 2, 0)).r);
        Src1 = max(Src1, SrcMip.Load(ta + int3(1, 2, 0)).r);
    }

    if (handleOddWidth && handleOddHeight)
    {
        Src1 = max(Src1, SrcMip.Load(ta + int3(2, 2, 0)).r);
    }

    OutMip1[DTid.xy] = Src1;
    
    if (numMips == 1)
        return;

    StoreDepth(GI, Src1);

    GroupMemoryBarrierWithGroupSync();

    OutMip1.GetDimensions(width, height);
    handleOddWidth = ((width & 1) != 0) && DTid.x == width - 3;
    handleOddHeight = ((height & 1) != 0) && DTid.y == height - 3;

    if ((GI & 0x9) == 0)
    {
        float Src2 = LoadDepth(GI + 0x01);
        float Src3 = LoadDepth(GI + 0x08);
        float Src4 = LoadDepth(GI + 0x09);
        Src1 = max(Src1, max(Src2, max(Src3, Src4)));

        if (handleOddWidth)
            Src1 = max(Src1, LoadDepth(GI + 0x02));
        if (handleOddHeight)
            Src1 = max(Src1, LoadDepth(GI + 0x08*2));
        if (handleOddWidth && handleOddHeight)
            Src1 = max(Src1, LoadDepth(GI + 0x08 * 2 + 0x1));


        OutMip2[DTid.xy / 2] = Src1;
        StoreDepth(GI, Src1);
    }

    if (numMips == 2)
        return;

    GroupMemoryBarrierWithGroupSync();

    if ((GI & 0x1B) == 0)
    {
        float Src2 = LoadDepth(GI + 0x02);
        float Src3 = LoadDepth(GI + 0x10);
        float Src4 = LoadDepth(GI + 0x12);
        Src1 = max(Src1, max(Src2, max(Src3, Src4)));

        OutMip3[DTid.xy / 4] = Src1;
        StoreDepth(GI, Src1);
    }

    if (numMips == 3)
        return;

    GroupMemoryBarrierWithGroupSync();

    if (GI == 0)
    {
        float4 Src2 = LoadDepth(GI + 0x04);
        float4 Src3 = LoadDepth(GI + 0x20);
        float4 Src4 = LoadDepth(GI + 0x24);
        Src1 = max(Src1, max(Src2, max(Src3, Src4)));

        OutMip4[DTid.xy / 8] = Src1;
    }

}