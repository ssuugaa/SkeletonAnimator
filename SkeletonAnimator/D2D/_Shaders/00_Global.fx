
//����������������������������������������������������������������������������������������������������������������������������������������������������������
// Common
//����������������������������������������������������������������������������������������������������������������������������������������������������������
cbuffer CB_PerFrame
{
    matrix View;
    matrix Projection;
}

matrix World;
Texture2D DiffuseMap;   // Texture2D�� 2���� �迭�̴�.
float4 Color;

// ���� 0�� �����ϸ� �ڵ����� 0�� �ٴ´�.
struct VertexInput
{
    float4 Position : POSITION0;
    float2 Uv : UV0;
};

// ���� 0�� �����ϸ� �ڵ����� 0�� �ٴ´�.
struct VertexOutput
{
    float4 Position : SV_POSITION0;
    // NDC ��ǥ ��ȯ �� �����͸� ����صд�!
    // SV�� ������ �ڵ���ȯ �ȵ�
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

// VS�� ���� PS�� �Ѿ�� Position�� ��ǥ�� NDC ��ǥ�� �������� ���Ѵ�!
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

//����������������������������������������������������������������������������������������������������������������������������������������������������������
// States
//����������������������������������������������������������������������������������������������������������������������������������������������������������
// ~~State �ڷ����� ���� �ɼ��� ��Ÿ����. desc�� ���� ���� 
// ���ø��� ������ �������°�
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    // �Ʒ� �� ���� ������ Ÿ�� ä����� ä���ش�.
    //AddressU = WRAP;
    //AddressV = WRAP;
};
SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
// �� 2������ Filter�� �����ߴ�.
// MIN_MAG_MIP_POINT�� Ȯ��/��� �� ��� ������ ���� ������ �״�� �����Ѵ�.
// MIN_MAG_MIP_LINEAR�� ���� ������ ��հ��� �־��ش�.


// �׸��� �׸��� ���?
RasterizerState CullMode_None
{
    // ���� �ð�������� ���� �ʾƵ� �׷��ش�.
    CullMode = None;
    // ���������� ������ �����ϰ� ���� �׷��ش�.
    // Clip�� �ȼ� ��⸦ �ǹ��Ѵ�. -�������� �ۿ� �ִ� �ȼ� ���
    // Cull�� �� ��⸦ �ǹ��Ѵ�. -�������� �ۿ� ��ģ �� ���
    DepthClipEnable = false;
    // ���� �ƴ� ������ ǥ�����ش�.
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

// �ϴ� ����� 2���� ���� �־�� �Ѵ�.
// ���� ��׶���� ĳ���� ���� ��ġ�� �ִ�.
// Dest�� �Ʒ��� / Src�� ����
// ���� : DestBlend * DestColor (OP) SrcBlend * SrcColor
// BlendOp���� �� �ִ��Ŀ� ���� ����� �ٲ��. - �⺻������ Add
