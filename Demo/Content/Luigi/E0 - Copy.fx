struct VS_OUTPUT
{
	float2 texCoord0 : TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR0;
};

uniform extern texture _texture;
uniform extern texture luigiCloth;
uniform extern float4 colorCloth;

sampler _textureS = sampler_state
{
	Texture = <_texture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

sampler luigiClothS = sampler_state
{
	Texture = <luigiCloth>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

PS_OUTPUT PS_main(VS_OUTPUT input)
{
	PS_OUTPUT result = (PS_OUTPUT)0;

	result.color = tex2D(_textureS, input.texCoord0);

	float4 temp = tex2D(luigiClothS, input.texCoord0);

	if (temp.a != 0.0f)
		result.color.rbg = (result.color * (1 - colorCloth)*0.5).rbg;

	return result;
}

technique DefaultTechnique
{
	pass Pass0
	{
		PixelShader = compile ps_2_0 PS_main();
	}
}