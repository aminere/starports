
Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	float4 texColor = txDiffuse.Sample(samLinear, input.uv);
	return texColor * input.color;
}

