
cbuffer MVP : register(b0)
{
	matrix mvp;
}

cbuffer TextureTransform : register(b1)
{
	matrix textureTransform;
}

cbuffer MaterialInfo : register(b2)
{
	float4 ambient;
}

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

VS_OUTPUT VSMain(float4 pos : aPosition, float2 uv : aUV)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.pos = mul( pos, mvp );
	output.uv = mul( uv, textureTransform );
	output.color = ambient;
    return output;
}
