



float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture g_Texture;

float g_fAlpha;
float			g_fMinRange = 0.f;
float			g_fMaxRange = 2.f;
float			g_fHPBar;
float4			g_vCamPosition;


sampler TextureSampler = sampler_state {
	texture = g_Texture;
	MINFILTER = linear;
	MAGFILTER = linear;
	MIPFILTER = linear;
};

sampler NoneSampler = sampler_state {
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

	Out.vColor = tex2D(NoneSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.a * g_fAlpha;

	return Out;
}

PS_OUT PS_MAIN_TERRAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(TextureSampler, In.vTexUV);

	// dot, cross, normalize

	float4		vFogColor = vector(1.f, 1.f, 1.f, 1.f);

	float		fDistance = length(g_vCamPosition - vector(In.vWorldPos, 1.f));


	//float		fFogPower = min((g_fMaxRange - max(fDistance - g_fMinRange, 0.f)), g_fMaxRange - g_fMinRange) / (g_fMaxRange - g_fMinRange);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= (vFogColor * fFogPower);

	return Out;
}

PS_OUT PS_MAIN_MONSTER(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(NoneSampler, In.vTexUV);

	// dot, cross, normalize

	float4		vFogColor = vector(1.f, 1.f, 1.f, 1.f);

	float		fDistance = length(g_vCamPosition - vector(In.vWorldPos, 1.f));

	//	float		fFogPower = min((g_fMaxRange - max(fDistance - g_fMinRange, 0.f)), g_fMaxRange - g_fMinRange) / (g_fMaxRange - g_fMinRange);

	float		fFogPower = max(fDistance - g_fMinRange, 0.f) / (g_fMaxRange - g_fMinRange);

	Out.vColor -= (vFogColor * fFogPower);
	Out.vColor.a = Out.vColor.a * g_fAlpha;

	if (Out.vColor.a == 0.f)
	{
		discard;
	}

	return Out;
}

PS_OUT PS_MAIN_UI(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(NoneSampler, In.vTexUV);

	Out.vColor.r = 0.5f;
	Out.vColor.gb = 0.f;
	Out.vColor.a = ((1.f - Out.vColor.a) * 0.5f) - g_fAlpha;

	if (Out.vColor.a == 0.f)
	{
		discard;
	}

	return Out;
}


PS_OUT PS_MAIN_HPBAR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(NoneSampler, In.vTexUV);

	if (In.vTexUV.x > g_fHPBar)
	{
		if (Out.vColor.r > 0.9f)
		{
			Out.vColor.r = 0.3137f;
			Out.vColor.g = 0.3607f;
			Out.vColor.b = 0.5019f;
		}
			
	}

	return Out;
}

PS_OUT PS_MAIN_QUESTUI(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(TextureSampler, In.vTexUV);
	Out.vColor.a = 0.7f;
	
	return Out;
}

PS_OUT PS_MAIN_Rect(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = tex2D(NoneSampler, In.vTexUV);
	
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
		AlphaFunc = GREATER;
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
	pass AlphaMonster
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_MONSTER();
	}
	pass HpBar
	{
		AlphaTestEnable = true;
		AlphaFunc = GREATER;
		AlphaRef = 100;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_HPBAR();
	}

	pass QuestUI
	{
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		BlendOp = Add;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_QUESTUI();
	}

	pass Rect
	{
		AlphaTestEnable = true;
		AlphaFunc = GREATER;
		AlphaRef = 100;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_Rect();
	}

}