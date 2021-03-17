
//─────────────────────────────────────────────────────────────────────────────
// Common
//─────────────────────────────────────────────────────────────────────────────
cbuffer CB_PerFrame
{
    matrix View;
    matrix Projection;
}

matrix World;
Texture2D DiffuseMap;   // Texture2D는 2차원 배열이다.
float4 Color;

// 뒤의 0은 생략하면 자동으로 0이 붙는다.
struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
};

// 뒤의 0은 생략하면 자동으로 0이 붙는다.
struct VertexOutput
{
    float4 Position : SV_POSITION0;
    // NDC 좌표 변환 전 데이터를 백업해둔다!
    // SV만 없으면 자동변환 안됨
    float4 oPosition : POSITION1;
    float4 wPosition : POSITION2;
    float2 Uv : UV0;
};

float4 WorldPosition(float4 position)
{
    return mul(position, World);
}
float4 ViewProjection(float4 position)
{
    position = mul(position, View);
    return mul(position, Projection);
}

// VS를 거쳐 PS로 넘어갈때 Position의 좌표가 NDC 좌표계 기준으로 변한다!
VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    output.oPosition = input.Position;

    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position;
    
    output.Position = ViewProjection(output.Position);
    
    output.Uv = input.Uv;

    return output;
}

//─────────────────────────────────────────────────────────────────────────────
// States
//─────────────────────────────────────────────────────────────────────────────
// ~~State 자료형은 전부 옵션을 나타낸다. desc와 같은 역할 
// 샘플링은 색깔을 가져오는것
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    // 아래 두 값을 넣으면 타일 채우듯이 채워준다.
    //AddressU = WRAP;
    //AddressV = WRAP;
};
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
// 위 2가지는 Filter를 적용했다.
// MIN_MAG_MIP_POINT는 확대/축소 시 비는 공간을 인접 색깔을 그대로 복붙한다.
// MIN_MAG_MIP_LINEAR는 인접 색깔의 평균값을 넣어준다.


// 그림을 그리는 방식?
RasterizerState CullMode_None
{
    // 점을 시계방향으로 찍지 않아도 그려준다.
    CullMode = None;
    // 프로젝션의 범위를 무시하고 전부 그려준다.
    // Clip은 픽셀 폐기를 의미한다. -프로젝션 밖에 있는 픽셀 폐기
    // Cull은 면 폐기를 의미한다. -프로젝션 밖에 걸친 면 폐기
    DepthClipEnable = false;
    // 면이 아닌 선으로 표시해준다.
    //FillMode = WireFrame;
};

BlendState AlphaBlend
{
    BlendEnable[0] = true;
    DestBlend[0] = INV_SRC_ALPHA;
    SrcBlend[0] = SRC_ALPHA;
    BlendOp[0] = Add;
    
    SrcBlendAlpha[0] = One;
    DestBlendAlpha[0] = One;
    RenderTargetWriteMask[0] = 0x0F;
};

// Macro Pass
#define P_VP(number, vs, ps) \
pass number \
{ \
    SetVertexShader(CompileShader(vs_5_0, vs())); \
    SetPixelShader(CompileShader(ps_5_0, ps())); \
 \
    SetBlendState(AlphaBlend, float4(0, 0, 0, 0), 0xFF); \
    SetRasterizerState(CullMode_None); \
}

// 일단 블랜드는 2개의 색이 있어야 한다.
// 현재 백그라운드와 캐릭터 색이 겹치고 있다.
// Dest는 아래쪽 / Src는 위쪽
// 계산식 : DestBlend * DestColor (OP) SrcBlend * SrcColor
// BlendOp값에 뭘 넣느냐에 따라 결과가 바뀐다. - 기본적으로 Add
