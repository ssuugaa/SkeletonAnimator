#include "00_Global.fx"

float Alpha;

float4 PS(VertexOutput input) : SV_TARGET0
{
	float4 color = DiffuseMap.Sample(PointSampler, input.Uv);
	color.a *= Alpha;

	return color;
}

technique11 T0
{
    P_VP(P0, VS, PS)
}