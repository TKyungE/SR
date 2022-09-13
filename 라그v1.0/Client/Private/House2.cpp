#include "stdafx.h"
#include "..\Public\House2.h"

#include "GameInstance.h"

CHouse2::CHouse2(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CHouse2::CHouse2(const CHouse2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CHouse2::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_IndexPos, pArg, sizeof(INDEXPOS));

	SetPos();

	return S_OK;
}

void CHouse2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pColliderCom->Set_Transform(m_pTransformCom1->Get_WorldMatrix(), 1.f);

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

void CHouse2::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom1->Get_State(CTransform::STATE_POSITION), m_pTransformCom1->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CHouse2::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(RectHouse_Render()))
		return E_FAIL;

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	if (g_bCollider)
		m_pColliderCom->Render();

	return S_OK;
}

HRESULT CHouse2::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBufferCube"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), (CComponent**)&m_pVIBufferCube)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBuffer1)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer1"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBuffer2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBuffer3)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer3"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBuffer4)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_HouseCube"), (CComponent**)&m_pTextureCom1)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_HouseRect"), (CComponent**)&m_pTextureCom2)))
		return E_FAIL;


	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom1, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform1"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom3, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform3"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom4, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform4"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom5, &TransformDesc)))
		return E_FAIL;
	
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CHouse2::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	/*m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);*/

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	return S_OK;
}

HRESULT CHouse2::Release_RenderState(void)
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

void CHouse2::SetPos(void)
{
	_float3 vPos = m_IndexPos.vPos;

	_float3 vScale = m_IndexPos.vScale;



	vPos.y += 0.5f * vScale.y;
	m_pTransformCom1->Set_Scaled(vScale);

	for (_uint i = 0; i < m_IndexPos.iTrun; ++i)
	{
		m_pTransformCom1->Turn(_float3(0.f, 1.f, 0.f), 1);
	}

	m_pTransformCom1->Set_State(CTransform::STATE_POSITION, vPos);

	// 가운데점
	_float3 vCenter = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vCenter.y += 0.5f * vScale.y;

	// 앞쪽 점 
	_float3 vFront = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vFront.y += 0.5f * vScale.y;
	vFront.z -= 0.5f * vScale.z;

	//임의의 벡터 
	_float3 vXYZ = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vXYZ.y += 1.25f * vScale.y;

	_float3 vFrontLook = vCenter - vFront;
	_float3 vFrontRight;

	D3DXVec3Cross(&vFrontRight, &vFrontLook, &_float3(0.f, 1.f, 0.f));


	D3DXVec3Normalize(&vFrontRight, &vFrontRight);
	vFrontRight *= vScale.x;
	m_pTransformCom2->Set_State(CTransform::STATE_RIGHT, vFrontRight);
	vCenter = vXYZ - vCenter;
	vFrontLook += vCenter;
	_float3 vFrontUp = vFrontLook;


	m_pTransformCom2->Set_State(CTransform::STATE_UP, vFrontUp);

	_float3 vLook;
	D3DXVec3Cross(&vLook, &vFrontUp, &vFrontRight);
	D3DXVec3Normalize(&vLook, &vLook);

	m_pTransformCom2->Set_State(CTransform::STATE_LOOK, vLook);

	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vFront);




	// 가운데점
	_float3 vCenter1 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vCenter1.y += 0.5f * vScale.y;

	// 뒷쪽 점 
	_float3 vBack = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vBack.y += 0.5f * vScale.y;
	vBack.z += 0.5f * vScale.z;

	//임의의 벡터 
	_float3 vXYZ1 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vXYZ1.y += 1.25f * vScale.y;

	_float3 vBackLook = vCenter1 - vBack;
	_float3 vBackRight;

	D3DXVec3Cross(&vBackRight, &vBackLook, &_float3(0.f, 1.f, 0.f));

	D3DXVec3Normalize(&vBackRight, &vBackRight);
	vBackRight *= vScale.x;
	m_pTransformCom3->Set_State(CTransform::STATE_RIGHT, vBackRight);
	vCenter1 = vXYZ1 - vCenter1;
	vBackLook += vCenter1;
	_float3 vBackUp = vBackLook;


	m_pTransformCom3->Set_State(CTransform::STATE_UP, vBackUp);

	_float3 vLook1;
	D3DXVec3Cross(&vLook1, &vBackUp, &vBackRight);
	D3DXVec3Normalize(&vLook1, &vLook1);

	m_pTransformCom3->Set_State(CTransform::STATE_LOOK, vLook1);

	m_pTransformCom3->Set_State(CTransform::STATE_POSITION, vBack);




	// 가운데점
	_float3 vCenter2 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vCenter2.y += 0.5f * vScale.y;

	// 오른쪽 점 
	_float3 vRight = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vRight.y += 0.5f * vScale.y;
	vRight.x += 0.5f * vScale.z;

	//임의의 벡터 
	_float3 vXYZ2 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vXYZ2.y += 1.25f * vScale.y;

	_float3 vRightLook = vCenter2 - vRight;
	_float3 vRightRight;

	D3DXVec3Cross(&vRightRight, &vRightLook, &_float3(0.f, 1.f, 0.f));

	D3DXVec3Normalize(&vRightRight, &vRightRight);
	vRightRight *= vScale.x;
	m_pTransformCom4->Set_State(CTransform::STATE_RIGHT, vRightRight);
	vCenter2 = vXYZ2 - vCenter2;
	vRightLook += vCenter1;
	_float3 vRightUp = vRightLook;


	m_pTransformCom4->Set_State(CTransform::STATE_UP, vRightUp);

	_float3 vLook2;
	D3DXVec3Cross(&vLook2, &vRightUp, &vRightRight);
	D3DXVec3Normalize(&vLook2, &vLook2);

	m_pTransformCom4->Set_State(CTransform::STATE_LOOK, vLook2);

	m_pTransformCom4->Set_State(CTransform::STATE_POSITION, vRight);




	// 가운데점
	_float3 vCenter3 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vCenter3.y += 0.5f * vScale.y;

	// 왼쪽 점 
	_float3 vLeft = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vLeft.y += 0.5f * vScale.y;
	vLeft.x -= 0.5f * vScale.z;

	//임의의 벡터 
	_float3 vXYZ3 = m_pTransformCom1->Get_State(CTransform::STATE_POSITION);
	vXYZ3.y += 1.25f * vScale.y;

	_float3 vLeftLook = vCenter3 - vLeft;
	_float3 vLeftRight;

	D3DXVec3Cross(&vLeftRight, &vLeftLook, &_float3(0.f, 1.f, 0.f));

	D3DXVec3Normalize(&vLeftRight, &vLeftRight);
	vLeftRight *= vScale.x;
	m_pTransformCom5->Set_State(CTransform::STATE_RIGHT, vLeftRight);
	vCenter3 = vXYZ3 - vCenter3;
	vLeftLook += vCenter1;
	_float3 vLeftUp = vLeftLook;


	m_pTransformCom5->Set_State(CTransform::STATE_UP, vLeftUp);

	_float3 vLook3;
	D3DXVec3Cross(&vLook3, &vLeftUp, &vLeftRight);
	D3DXVec3Normalize(&vLook3, &vLook3);

	m_pTransformCom5->Set_State(CTransform::STATE_LOOK, vLook3);

	m_pTransformCom5->Set_State(CTransform::STATE_POSITION, vLeft);

}

HRESULT CHouse2::RectHouse_Render(void)
{
	if (FAILED(m_pTransformCom1->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom1->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferCube->Render();



	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBuffer1->Render();

	

	if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBuffer2->Render();


	if (FAILED(m_pTransformCom4->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBuffer3->Render();


	if (FAILED(m_pTransformCom5->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBuffer4->Render();

	return S_OK;
}

CHouse2 * CHouse2::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CHouse2* pInstance = new CHouse2(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHouse2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHouse2::Clone(void * pArg)
{
	CHouse2* pInstance = new CHouse2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHouse2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CHouse2::Get_World(void)
{
	return m_pTransformCom1->Get_WorldMatrix();
}

void CHouse2::Free(void)
{
	__super::Free();

	Safe_Release(m_pColliderCom);

	Safe_Release(m_pTransformCom1);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBuffer1);
	Safe_Release(m_pTextureCom1);

	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBuffer2);
	Safe_Release(m_pTextureCom2);

	Safe_Release(m_pTransformCom3);
	Safe_Release(m_pVIBuffer3);


	Safe_Release(m_pTransformCom4);
	Safe_Release(m_pVIBuffer4);
	
	Safe_Release(m_pTransformCom5);

	Safe_Release(m_pVIBufferCube);
	
}
