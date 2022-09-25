


float4x4	g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
textureCUBE g_Texture;
float g_fAlpha;

sampler TextureSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};

struct VS_IN
{
	float3 vPosition : POSITION;
	float3 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float3 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix).xyz;
	return Out;
}

struct PS_IN
{
	float4 vPosition : POSITION;
	float3 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;

};

struct PS_OUT
{
	vector vColor : COLOR0;
};
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	Out.vColor.rgb = 0.f;

	return Out;
}

PS_OUT PS_TREE_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	Out.vColor.a = Out.vColor.a * g_fAlpha;

	if (Out.vColor.a == 0.f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_SKYBOX2(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = texCUBE(TextureSampler, In.vTexUV);

	Out.vColor.r = (Out.vColor.r * 0.5f) + (0.9725f * 0.8f);
	Out.vColor.g = (Out.vColor.g * 0.5f) + (0.4745f * 0.75f);
	Out.vColor.b = (Out.vColor.b * 0.5f) + (0.054f * 0.75f);

	return Out;
}


technique DefaultTechnique
{
	pass SkyBox
	{
		ZEnable = true;
		ZWriteEnable = false;
		CullMode = cw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Tree
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_TREE_MAIN();
	}
	pass SkyBox2
	{
		ZEnable = true;
		ZWriteEnable = false;
		CullMode = cw;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_SKYBOX2();
	}
}