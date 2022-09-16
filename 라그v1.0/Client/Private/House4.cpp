#include "stdafx.h"
#include "..\Public\House4.h"
#include "GameInstance.h"


CHouse4::CHouse4(LPDIRECT3DDEVICE9 _pGraphic_Device)
	:CGameObject(_pGraphic_Device)
{
	ZeroMemory(&m_IndexPos, sizeof(INDEXPOS));
}

CHouse4::CHouse4(const CHouse4 & rhs)
	:CGameObject(rhs)
{
}

HRESULT CHouse4::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse4::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//memcpy(&m_IndexPos, pArg, sizeof(INDEXPOS));

	//SetPos();

	//============================================================================================================

	_float3 vScale = _float3(3.f, 3.f, 3.f);
	m_pTransformCom->Set_Scaled(vScale);
	_float3 vPos = _float3(14.5f, 0.f, 13.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	//============================================================================================================

	
	m_pTransformCom2->Set_Scaled(vScale);
	_float3 vPos2 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos2.x -= 0.5f * vScale.x;
	vPos2.z += 0.325f * vScale.z;
	m_pTransformCom2->Turn(_float3(0.f, 1.f, 0.f), 1);

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos2);


	////============================================================================================================



	m_pTransformCom3->Set_Scaled(vScale);
	_float3 vPos3 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos3.x += 0.5f * vScale.x;
	vPos3.z += 0.325f * vScale.z;
	m_pTransformCom3->Turn(_float3(0.f, 1.f, 0.f), 1);

	m_pTransformCom3->Set_State(CTransform::STATE_POSITION, vPos3);

	////============================================================================================================


	m_pTransformCom4->Set_Scaled(vScale);
	_float3 vPos4 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos4.z += 0.675f * vScale.z;

	m_pTransformCom4->Set_State(CTransform::STATE_POSITION, vPos4);

	////============================================================================================================



	m_pTransformCom5->Set_Scaled(vScale);
	_float3 vPos5 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos5.z -= 0.001f;
	vPos5.y += 0.45f * vScale.y;

	m_pTransformCom5->Set_State(CTransform::STATE_POSITION, vPos5);

	//============================================================================================================


	////============================================================================================================



	m_pTransformCom6->Set_Scaled(vScale);
	_float3 vPos6 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos6.z += 0.675f * vScale.z;
	vPos6.y += 0.45f * vScale.y;

	m_pTransformCom6->Set_State(CTransform::STATE_POSITION, vPos6);


	////============================================================================================================

	_float3 vScale2 = _float3(2.f, 2.6f, 3.f);
	m_pTransformCom7->Set_Scaled(vScale2);
	_float3 vPos7 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos7.x -= 0.5f * vScale.x;
	vPos7.z += 0.325f * vScale.z;
	vPos7.y += 0.45f * vScale.y;

	m_pTransformCom7->Turn(_float3(0.f, 1.f, 0.f), 1);
	m_pTransformCom7->Turn(_float3(0.f, 0.f, 1.f), -0.56);

	m_pTransformCom7->Set_State(CTransform::STATE_POSITION, vPos7);



	////============================================================================================================



	m_pTransformCom8->Set_Scaled(vScale2);
	_float3 vPos8 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos8.x += 0.5f * vScale.x;
	vPos8.z += 0.325f * vScale.z;
	vPos8.y += 0.45f * vScale.y;

	m_pTransformCom8->Turn(_float3(0.f, 1.f, 0.f), 1);
	m_pTransformCom8->Turn(_float3(0.f, 0.f, 1.f), 0.56);

	m_pTransformCom8->Set_State(CTransform::STATE_POSITION, vPos8);



	////============================================================================================================
	return S_OK;
}

void CHouse4::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	_float4x4 WorldMatrix;
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.z += 1.f;
	
	WorldMatrix.m[1][1] = 1.f;

	WorldMatrix.m[2][2] = 2.f;

	*(_float3*)&WorldMatrix.m[3][0] = vPos;
	m_pColliderCom->Set_Transform(WorldMatrix, 1.f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_OBJECT, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CHouse"));
		return;
	}

	Safe_Release(pInstance);


}

void CHouse4::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CHouse4::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;


	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferCom->Render();



	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(2)))
		return E_FAIL;
	m_pVIBufferCom2->Render();



	if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(2)))
		return E_FAIL;
	m_pVIBufferCom3->Render();



	if (FAILED(m_pTransformCom4->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;
	m_pVIBufferCom4->Render();



	if (FAILED(m_pTransformCom5->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(3)))
		return E_FAIL;
	m_pVIBufferCom5->Render();




	if (FAILED(m_pTransformCom6->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(3)))
		return E_FAIL;
	m_pVIBufferCom6->Render();



	if (FAILED(m_pTransformCom7->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(4)))
		return E_FAIL;
	m_pVIBufferCom7->Render();


	if (FAILED(m_pTransformCom8->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(4)))
		return E_FAIL;
	m_pVIBufferCom8->Render();


	if (FAILED(Release_RenderState()))
		return E_FAIL;

	if (g_bCollider)
		m_pColliderCom->Render();
}

HRESULT CHouse4::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer1"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom4)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer4"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom5)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer5"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom6)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer6"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom7)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer7"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom8)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_House4"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform1"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom4, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform4"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom5, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform5"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom6, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform6"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom7, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform7"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom8, &TransformDesc)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse4::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
/*
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);*/
	return S_OK;
}

HRESULT CHouse4::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	return S_OK;
}

void CHouse4::SetPos(void)
{
	_float3 vPos = _float3(15.f,0.f,10.f);

	_float3 vScale = _float3(3.f, 3.f, 3.f);

	_uint iTurn = 0;

	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom2->Set_Scaled(vScale);
	m_pTransformCom3->Set_Scaled(vScale);
	m_pTransformCom4->Set_Scaled(vScale);

	for (_uint i = 0; i < iTurn; ++i)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 1);
		m_pTransformCom2->Turn(_float3(0.f, 1.f, 0.f), 1);
	}

	m_pTransformCom3->Turn(_float3(0.f, 1.f, 0.f), 1);
	m_pTransformCom4->Turn(_float3(0.f, 1.f, 0.f), 1);

	for (_uint i = 0; i < iTurn; ++i)
	{
		m_pTransformCom3->Turn(_float3(0.f, 1.f, 0.f), 1);
		m_pTransformCom4->Turn(_float3(0.f, 1.f, 0.f), 1);
	}


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	//==============================================================
	//_float3 vCenter = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//vCenter.x -= 0.3f;

	//m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos);

	////==============================================================
	//_float3 vCenter = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	//vCenter.x += 1.1f;
	//m_pTransformCom3->Set_State(CTransform::STATE_POSITION, vCenter);


	////==============================================================

	//m_pTransformCom4->Set_State(CTransform::STATE_POSITION, vPos);


	////==============================================================


	//m_pTransformCom5->Set_State(CTransform::STATE_POSITION, vPos);
	//
	////==============================================================

	//m_pTransformCom6->Set_State(CTransform::STATE_POSITION, vPos);

}

HRESULT CHouse4::RectHouse_Render(void)
{
	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	//m_pVIBufferCom->Render();



	/*if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;
	m_pVIBufferCom2->Render();
*/


	/*if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(2)))
		return E_FAIL;
	m_pVIBufferCom3->Render();*/


	/*if (FAILED(m_pTransformCom4->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(3)))
		return E_FAIL;
	m_pVIBufferCom4->Render();


	if (FAILED(m_pTransformCom5->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(4)))
		return E_FAIL;
	m_pVIBufferCom5->Render();


	if (FAILED(m_pTransformCom6->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(5)))
		return E_FAIL;
	m_pVIBufferCom6->Render();*/

	return S_OK;
}

CHouse4 * CHouse4::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CHouse4* pInstance = new CHouse4(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHouse3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHouse4::Clone(void * pArg)
{
	CHouse4* pInstance = new CHouse4(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHouse3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CHouse4::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CHouse4::Free(void)
{
	__super::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pTransformCom4);
	Safe_Release(m_pTransformCom5);
	Safe_Release(m_pTransformCom6);
	Safe_Release(m_pTransformCom7);
	Safe_Release(m_pTransformCom8);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pVIBufferCom3);
	Safe_Release(m_pVIBufferCom4);
	Safe_Release(m_pVIBufferCom5);
	Safe_Release(m_pVIBufferCom6);
	Safe_Release(m_pVIBufferCom7);
	Safe_Release(m_pVIBufferCom8);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColliderCom);
}
