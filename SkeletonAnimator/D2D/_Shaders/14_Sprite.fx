#include "00_Global.fx"

float4 PS(VertexOutput input) : SV_TARGET0
{
    return DiffuseMap.Sample(PointSampler, input.Uv);
}

technique11 T0
{
    P_VP(P0, VS, PS)
}