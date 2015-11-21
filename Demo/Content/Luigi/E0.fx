struct VS_OUTPUT
{
	float2 texCoord0 : TEXCOORD0;
	float4 color0 : COLOR0;
};

uniform extern texture _texture;
uniform extern texture luigiCloth;
uniform extern float hue;

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

float3 RGBToHSV(float R, float G, float B)
{
	float H, S, V;

	float CMax = max(max(R, G), B);
	float CMin = min(min(R, G), B);
	float delta = CMax - CMin;

	if (delta != 0)
	{
		if (CMax == R)
			H = (G - B) / delta % 6;
		else if (CMax == G)
			H = (B - R) / delta + 2;
		else
			H = (R - G) / delta + 4;
	}
	else
		H = 0.0f;

	H *= 60.0f;

	if (CMax == 0.0f)
		S = 0.0f;
	else
		S = delta / CMax;

	V = CMax;

	return float3(H, S, V);
}

float3 HSVToRGB(float H, float S, float V)
{
	float R, B, G;

	float C = V*S;
	float X = C*(1 - abs(H / 60 % 2 - 1));
	float m = V - C;

	R = B = G = m;

	if (H >= 0.0f && H < 60.0f)
	{
		R += C;
		G += X;
	}
	else if (H < 120.0f)
	{
		R += X;
		G += C;
	}
	else if (H < 180.0f)
	{
		G += C;
		B += X;
	}
	else if (H < 240.0f)
	{
		G += X;
		B += C;
	}
	else if (H < 300.0f)
	{
		R += X;
		B += C;
	}
	else if (H < 360.0f)
	{
		R += C;
		B += X;
	}
	else
	{
		R = 0.0f;
		G = 0.0f;
		B = 0.0f;
	}

	return float3(R, G, B);
}

float4 PS_main(VS_OUTPUT input) : COLOR
{
	float4 result = tex2D(_textureS, input.texCoord0);
	float4 temp = tex2D(luigiClothS, input.texCoord0);

	if (temp.a)
	{
		result.rgb = RGBToHSV(result.r, result.g, result.b);

		result.x += hue;
		if (result.x > 359.0f)
			result.x -= 359.0f;

		if (result.x < 0.0f)
			result.x += 359.0f;

		result.rgb = HSVToRGB(result.x, result.y, result.z);
	}

	return result;
}

technique DefaultTechnique
{
	pass Pass0
	{
		PixelShader = compile ps_2_a PS_main();
	}
}