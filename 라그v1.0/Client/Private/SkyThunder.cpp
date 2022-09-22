#include "stdafx.h"
#include "..\Public\SkyThunder.h"
#include "GameInstance.h"


CSkyThunder::CSkyThunder(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CSkyThunder::CSkyThunder(const CSkyThunder & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkyThunder::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyThunder::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tInfo.bDead = false;
	m_tInfo.fX = 0.5f;
	m_tInfo.iDmg = 1111;
	m_tInfo.iMoney = 11;
	Set_vPos();
	if (!m_tInfo.pTarget)
		Set_Dead();
	return S_OK;
}

void CSkyThunder::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 7.0f)
		Set_Dead();
	
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);


	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_PLAYERSKILL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CSkyThunder"));
		return;
	}

	Safe_Release(pInstance);
}

void CSkyThunder::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float Distance = 0.f;
	if (!m_tInfo.pTarget)
	{
		vLook = m_tInfo.vTargetPos - vPosition;
		vPosition += *D3DXVec3Normalize(&vLook, &vLook) * 12.f * fTimeDelta;
	}
	if (!m_tInfo.pTarget->Get_Info().bDead)
	{
		vLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - vPosition;
		m_tInfo.vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition += *D3DXVec3Normalize(&vLook, &vLook) * 12.f * fTimeDelta;
	}
	else
	{
		Distance = D3DXVec3Length(&(m_tInfo.vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		if (Distance <= 0.1f)
		{
			Set_Dead();
			return;
		}
		vLook = m_tInfo.vTargetPos - vPosition;
		vPosition += *D3DXVec3Normalize(&vLook, &vLook) * 8.f * fTimeDelta;
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);


	CheckColl();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);
	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_ALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CSkyThunder::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;


	//상태에 따라 바인드하는 함수
	TextureRender();

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();
	if (FAILED(m_pTransformCom2->Bind_OnGraphicDev()))
		return E_FAIL;

	m_pVIBufferCom2->Render();
	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();
	if (g_bCollider)
		m_pColliderCom->Render();
	return S_OK;
}



HRESULT CSkyThunder::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(0)))
			return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CSkyThunder::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CSkyThunder::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}


HRESULT CSkyThunder::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkyThunder"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer2"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom2)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Transform2"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom2, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkyThunder::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CSkyThunder::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CSkyThunder * CSkyThunder::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CSkyThunder*	pInstance = new CSkyThunder(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CSkyThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkyThunder::Clone(void* pArg)
{
	CSkyThunder*	pInstance = new CSkyThunder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CSkyThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CSkyThunder::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CSkyThunder::Free()
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pVIBufferCom2);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
void CSkyThunder::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_MONSTER, TEXT("Com_Collider"), &pTarget))
	{
		pTarget->Set_Hp(m_tInfo.iDmg);
		pTarget->Set_Hit(m_tInfo.iDmg, *(_float3*)&pTarget->Get_World().m[3][0]);
		if (pTarget->Get_Info().iHp <= 0)
			pTarget->Set_Dead();
		Set_Dead();
	}

	Safe_Release(pInstance);
}
void CSkyThunder::Set_vPos()
{
	_float3 vScale = { 0.5f,0.5f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom2->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	_float3 vUp = { 0.f,1.f,0.f };
	m_pTransformCom2->Turn(vUp, 1.f);
}