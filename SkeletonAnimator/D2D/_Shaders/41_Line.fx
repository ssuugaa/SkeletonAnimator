#include "00_Global.fx"

float4 LineColor = float4(1, 1, 1, 0.15f);
float4 SymbolColor = float4(1, 1, 1, 1);
float4 EndColor = float4(1, 0, 0, 1);

// 뒤의 0은 생략하면 자동으로 0이 붙는다.
struct VertexInput_Collider
{
    float4 Position : POSITION;
};

// 뒤의 0은 생략하면 자동으로 0이 붙는다.
struct VertexOutput_Collider
{
    float4 Position : SV_POSITION;
};

VertexOutput_Collider VS_Collider(VertexInput_Collider input)
{
    VertexInput_Collider output;

    output.Position = WorldPosition(input.Position);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

// 뒤의 0은 생략하면 자동으로 0이 붙는다.
float4 PS0(VertexOutput_Collider input) : SV_TARGET
{
    return LineColor;
}
float4 PS1(VertexOutput_Collider input) : SV_TARGET
{
    return SymbolColor;
}
float4 PS2(VertexOutput_Collider input) : SV_TARGET
{
    return EndColor;
}

technique11 T0
{
    P_VP(P0, VS_Collider, PS0)
    P_VP(P1, VS_Collider, PS1)
    P_VP(P2, VS_Collider, PS2)
}