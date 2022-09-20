



float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;

float g_fAlpha;
float			g_fMinRange = 1.f;
float			g_fMaxRange = 3.f;

float4			g_vCamPosition;


sampler TextureSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};
sampler CharNpcSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = POINT;
	MAGFILTER = POINT;
	MIPFILTER = POINT;
	
};

sampler MonsterSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = NONE;
	MAGFILTER = NONE;
	MIPFILTER = NONE;

};
struct VS_IN
{
	float3 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix			matWV, matWVP;

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
	float2 vTexUV : TEXCOORD0;
	float3 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(TextureSampler, In.vTexUV);


	return Out;
}


PS_OUT PS_NPC(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(CharNpcSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a * g_fAlpha;

	return Out;
}

PS_OUT PS_MAIN_TERRAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(TextureSampler, In.vTexUV);

	// dot, cross, normalize

	float4		vFogColor = Out.vColor;

	float		fDistance = length(g_vCamPosition - vector(In.vWorldPos, 1.f));

	 float		fFogPower = min((g_fMaxRange - max(fDistance - g_fMinRange, 0.f)), g_fMaxRange - g_fMinRange) / (g_fMaxRange - g_fMinRange);

	//float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor += vFogColor * fFogPower;

	return Out;
}

PS_OUT PS_MAIN_MONSTER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(MonsterSampler, In.vTexUV);

	// dot, cross, normalize

	float4		vFogColor = Out.vColor;

	float		fDistance = length(g_vCamPosition - vector(In.vWorldPos, 1.f));

	float		fFogPower = min((g_fMaxRange - max(fDistance - g_fMinRange, 0.f)), g_fMaxRange - g_fMinRange) / (g_fMaxRange - g_fMinRange);

	//float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor += vFogColor * fFogPower;

	return Out;
}

PS_OUT PS_MAIN_UI(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(MonsterSampler, In.vTexUV);


	Out.vColor.g = 1.f;

	return Out;
}

technique DefaultTechnique
{
	pass DefaultPass
	{
		/*AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;*/

		AlphaTestEnable = true;
		AlphaFunc= GREATER; 
		AlphaRef = 100;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
	pass NpcCharPass
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		/*AlphaTestEnable = true;
		AlphaFunc = GREATER;
		AlphaRef = 100;*/

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_NPC();
	}
	pass Terrain
	{
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_TERRAIN();
	}
	pass Monster
	{
		AlphaTestEnable = true;
		AlphaFunc = GREATER;
		AlphaRef = 100;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_MONSTER();
	}
	pass Shadow
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_MONSTER();
	}

	pass UI
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_UI();
	}
}