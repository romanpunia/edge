#include "vertex-in.hlsl"
#include "render-buffer.hlsl"

Texture2D Input1 : register(t1);
SamplerState State : register(s0);

cbuffer ViewBuffer : register(b3)
{
	matrix SliceViewProjection[6];
	float3 Position;
	float Distance;
};

struct VertexCubeOut
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 RawPosition : TEXCOORD1;
	uint RenderTarget : SV_RenderTargetArrayIndex;
};

struct VertexOut
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float4 RawPosition : TEXCOORD1;
};

[maxvertexcount(18)]
void GS(triangle VertexOut Input[3], inout TriangleStream<VertexCubeOut> Stream)
{
	VertexCubeOut Output = (VertexCubeOut)0;
	for (Output.RenderTarget = 0; Output.RenderTarget < 6; Output.RenderTarget++)
	{
		Output.Position = mul(Input[0].Position, SliceViewProjection[Output.RenderTarget]);
		Output.RawPosition = Input[0].RawPosition;
		Output.TexCoord = Input[0].TexCoord;
		Stream.Append(Output);

		Output.Position = mul(Input[1].Position, SliceViewProjection[Output.RenderTarget]);
		Output.RawPosition = Input[1].RawPosition;
		Output.TexCoord = Input[1].TexCoord;
		Stream.Append(Output);

		Output.Position = mul(Input[2].Position, SliceViewProjection[Output.RenderTarget]);
		Output.RawPosition = Input[2].RawPosition;
		Output.TexCoord = Input[2].TexCoord;
		Stream.Append(Output);

		Stream.RestartStrip();
	}
}

VertexOut VS(VertexIn Input)
{
	VertexOut Output = (VertexOut)0;
	Output.Position = Output.RawPosition = mul(Input.Position, World);
	Output.TexCoord = Input.TexCoord * TexCoord;
	return Output;
}

float4 PS(VertexOut Input) : SV_TARGET0
{
	float Alpha = (1.0f - Diffusion.y);
	[branch] if (SurfaceDiffuse > 0)
		Alpha *= Input1.Sample(State, Input.TexCoord * TexCoord).w;

	[branch] if (Alpha < Diffusion.x)
		discard;

	return float4(length(Input.RawPosition.xyz - Position) / Distance, Alpha, 1.0f, 1.0f);
};