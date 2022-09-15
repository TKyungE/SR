#include "stdafx.h"
#include "..\Public\FireSpear.h"
#include "GameInstance.h"
#include "Fire.h"

CFireSpear::CFireSpear(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CFireSpear::CFireSpear(const CFireSpear & rhs)
	: CGameObject(rhs)
{
}

HRESULT CFireSpear::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireSpear::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 10;
	m_tFrame.fFrameSpeed = 0.08f;
	m_tInfo.bDead = false;
	m_tInfo.fX = 1.f;
	m_tInfo.iDmg = 66;
	m_tInfo.iMoney = 33;
	Set_vPos();
	return S_OK;
}

void CFireSpear::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	m_tInfo.bDead = false;
	OnTerrain();

	m_pTransformCom->Go_Down(fTimeDelta);
	m_pTransformCom2->Go_Down(fTimeDelta);
	Move_Frame(fTimeDelta);
	m_fDeadTime += fTimeDelta;
	if (m_fDeadTime > 2.f)
	{
		Set_Dead();
	}
	if (m_tInfo.bDead)
	{
		Create_Fire(TEXT("Layer_Skill"));
	}
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_MONSTERSKILL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CThunderSword"));
		return;
	}

	Safe_Release(pInstance);
}

void CFireSpear::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_CollTime += fTimeDelta;
	Motion_Change();
	if (m_CollTime > 0.3f)
	{
		CheckColl();
	}
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFireSpear::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;
	Off_SamplerState();


	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
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
HRESULT CFireSpear::Create_Fire(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.vPos.y += 0.5f;
	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Fire"), LEVEL_STATIC, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CFireSpear::Set_vPos()
{
	m_tInfo.vPos.y += 5.5f;
	_float3 vScale = { 2.f,3.f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom2->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	_float3 vUp = { 0.f,1.f,0.f };
	m_pTransformCom2->Turn(vUp, 1.f);
}

void CFireSpear::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 10;
			m_tFrame.fFrameSpeed = 0.08f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CFireSpear::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		m_tFrame.iFrameStart = m_pTextureCom->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}

}


void CFireSpear::OnTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(m_tInfo.iLevelIndex, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom2->Set_State(CTransform::STATE_POSITION, vPosition);

	if (vPosition.y < pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 1.1f))
		Set_Dead();

	Safe_Release(pGameInstance);
}
HRESULT CFireSpear::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireSpear"), (CComponent**)&m_pTextureCom)))
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
HRESULT CFireSpear::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CFireSpear::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
HRESULT CFireSpear::SetUp_RenderState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CFireSpear::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CFireSpear * CFireSpear::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFireSpear*	pInstance = new CFireSpear(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFireSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireSpear::Clone(void* pArg)
{
	CFireSpear*	pInstance = new CFireSpear(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFireSpear"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CFireSpear::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CFireSpear::Free()
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
void CFireSpear::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
	{
		pTarget->Set_Hp(m_tInfo.iDmg);
		pTarget->Set_Hit(m_tInfo.iDmg, Get_CollisionPos(pTarget, this));
		if (pTarget->Get_Info().iHp <= 0)
			pTarget->Set_Dead();
	//	Set_Dead();
	}

	Safe_Release(pInstance);
}
_float3 CFireSpear::Get_CollisionPos(CGameObject * pDest, CGameObject * pSour)
{
	_float3 vLook = *(_float3*)&pDest->Get_World().m[3][0] - *(_float3*)&pSour->Get_World().m[3][0];
	D3DXVec3Normalize(&vLook, &vLook);

	vLook = vLook * 0.5f;

	_float Angle = D3DXVec3Dot(&vLook, (_float3*)&pSour->Get_World().m[1][0]);
	_float3 SourUp = *(_float3*)&pSour->Get_World().m[1][0];
	_float3 Proj = (Angle / D3DXVec3Length(&SourUp) * D3DXVec3Length(&SourUp)) * *(_float3*)&pSour->Get_World().m[1][0];

	_float3 CollisionPos = *(_float3*)&pSour->Get_World().m[3][0] + Proj;
//	CollisionPos.y -= 0.8f;
	CollisionPos.x = pDest->Get_World().m[3][0];

	return CollisionPos;
}