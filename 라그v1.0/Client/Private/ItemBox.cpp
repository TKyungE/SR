#include "stdafx.h"
#include "..\Public\ItemBox.h"
#include "GameInstance.h"


CItemBox::CItemBox(LPDIRECT3DDEVICE9 _pGraphic_Device)
	:CGameObject(_pGraphic_Device)
{
	ZeroMemory(&m_IndexPos, sizeof(INDEXPOS));
}

CItemBox::CItemBox(const CItemBox & rhs)
	:CGameObject(rhs)
{
}

HRESULT CItemBox::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemBox::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	//memcpy(&m_IndexPos, pArg, sizeof(INDEXPOS));

	//SetPos();

	//============================================================================================================

	_float3 vScale = _float3(0.5f, 0.5f, 0.5f);
	m_pTransformCom->Set_Scaled(vScale);
	_float3 vPos = _float3(6.f, 0.f, 22.f);
	vPos.y += 0.5f * vScale.y;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	//============================================================================================================

	_float3 vScale2 = _float3(0.5f, 0.175f, 0.5f);
	m_pTransformCom2->Set_Scaled(vScale2);
	_float3 vPos2 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos2.z -= 0.5f * vScale.z;
	vPos2.y -= 0.5f * vScale.y;
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPos2);


	////============================================================================================================

	_float3 vScale3 = _float3(0.72f, 1.f, 1.f);

	m_pTransformCom3->Set_Scaled(vScale3);
	_float3 vPos3 = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos3.z -= 0.5f * vScale.z;
	vPos3.y -= 0.2f * vScale.y;

	m_pTransformCom3->Turn(_float3(1.f, 0.f, 0.f), 0.6f);
	m_pTransformCom3->Set_State(CTransform::STATE_POSITION, vPos3);

	////============================================================================================================



	////============================================================================================================


	//============================================================================================================



	////============================================================================================================



	////============================================================================================================





	////============================================================================================================





	////============================================================================================================
	return S_OK;
}

void CItemBox::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 1.f);

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

void CItemBox::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	//if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	//{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	//}
	Safe_Release(pInstance);
}

HRESULT CItemBox::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;


	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom2->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferComCube->Render();



	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;
	m_pVIBufferCom->Render();



	if (FAILED(m_pTransformCom3->Bind_OnGraphicDev()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(1)))
		return E_FAIL;
	m_pVIBufferCom2->Render();





	if (FAILED(Release_RenderState()))
		return E_FAIL;

	if (g_bCollider)
		m_pColliderCom->Render();
}

HRESULT CItemBox::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer1"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect2"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer5"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), (CComponent**)&m_pVIBufferComCube)))
		return E_FAIL;


	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemBox"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture1"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemBox_Cube"), (CComponent**)&m_pTextureCom2)))
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

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemBox::SetUp_RenderState(void)
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

HRESULT CItemBox::Release_RenderState(void)
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

void CItemBox::SetPos(void)
{
	_float3 vPos = _float3(15.f,0.f,10.f);

	_float3 vScale = _float3(3.f, 3.f, 3.f);

	_uint iTurn = 0;

	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom2->Set_Scaled(vScale);
	m_pTransformCom3->Set_Scaled(vScale);

	for (_uint i = 0; i < iTurn; ++i)
	{
		m_pTransformCom->Turn(_float3(0.f, 1.f, 0.f), 1);
		m_pTransformCom2->Turn(_float3(0.f, 1.f, 0.f), 1);
	}

	m_pTransformCom3->Turn(_float3(0.f, 1.f, 0.f), 1);


	for (_uint i = 0; i < iTurn; ++i)
	{
		m_pTransformCom3->Turn(_float3(0.f, 1.f, 0.f), 1);
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

HRESULT CItemBox::RectHouse_Render(void)
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

CItemBox * CItemBox::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CItemBox* pInstance = new CItemBox(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CHouse3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemBox::Clone(void * pArg)
{
	CItemBox* pInstance = new CItemBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CHouse3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CItemBox::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CItemBox::Free(void)
{
	__super::Free();
}
