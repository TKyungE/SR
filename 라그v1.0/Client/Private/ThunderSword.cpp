#include "stdafx.h"
#include "..\Public\ThunderSword.h"
#include "GameInstance.h"
#include "Layer.h"
#include "StatInfo.h"
#include "Camera_Dynamic.h"

CThunderSword::CThunderSword(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CThunderSword::CThunderSword(const CThunderSword & rhs)
	: CGameObject(rhs)
{
}

HRESULT CThunderSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThunderSword::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	_float3 vScale = { 1.5f,1.5f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.2f);

	//m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.2f);
	
	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 16;
	m_tFrame.fFrameSpeed = 0.06f;
	m_tInfo.bDead = false;
	m_tInfo.fX = 0.5f;
	m_tInfo.iDmg = 79 + (m_tInfo.pTarget->Get_Info().iDmg / 2);
	m_tInfo.iMoney = 22;


	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);
	return S_OK;
}

void CThunderSword::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	Move_Frame(fTimeDelta);
	m_pTransformCom->Go_Down(fTimeDelta);
	if(m_pTransformCom->Get_State(CTransform::STATE_POSITION).y < 0.f)
		Set_Dead();

	m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.2f);

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_PLAYERSKILL, this)))
	{
		ERR_MSG(TEXT("Failed to Add CollisionGroup : CThunderSword"));
		return;
	}
	
	Safe_Release(pInstance);
	
}

void CThunderSword::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	

	Motion_Change();
	OnBillboard();
	CheckColl();

	Compute_CamDistance(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pInstance);
	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CThunderSword::Render()
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

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();

	if (g_bCollider)
		m_pColliderCom->Render();

	return S_OK;
}


void CThunderSword::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 16;
			m_tFrame.fFrameSpeed = 0.06f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CThunderSword::Move_Frame(_float fTimeDelta)
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
void CThunderSword::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale = { 1.5f,1.5f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	//m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
void CThunderSword::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_MONSTER, TEXT("Com_Collider"), &pTarget))
	{
		_float fCri = _float(rand() % 100 + 1);
		_float fLUK = (_float)dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iLUK / 2.f;

		if (fCri <= fLUK)
		{
			pTarget->Set_Hp(m_tInfo.iDmg * 2);
			pTarget->Set_Hit(m_tInfo.iDmg * 2, *(_float3*)&pTarget->Get_World().m[3][0]);
			CGameObject::INFO tInfo;
			tInfo.pTarget = pTarget;
			tInfo.vPos = *(_float3*)&pTarget->Get_World().m[3][0];
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit2"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			dynamic_cast<CCamera_Dynamic*>(pInstance->Find_Layer(m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Camera"))->Get_Objects().front())->CriHit();
		}
		else
		{
			pTarget->Set_Hp(m_tInfo.iDmg);
			pTarget->Set_Hit(m_tInfo.iDmg, Get_CollisionPos(pTarget, this));
		}
		if (pTarget->Get_Info().iHp <= 0)
			pTarget->Set_Dead();
		Set_Dead();
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_BOSS, TEXT("Com_Collider"), &pTarget))
	{
		if (pTarget->Get_Info().iMp == 0)
		{
			_float fCri = _float(rand() % 100 + 1);
			_float fLUK = (_float)dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iLUK / 2.f;

			if (fCri <= fLUK)
			{
				pTarget->Set_Hp(m_tInfo.iDmg * 2);
				pTarget->Set_Hit(m_tInfo.iDmg * 2, *(_float3*)&pTarget->Get_World().m[3][0]);
				CGameObject::INFO tInfo;
				tInfo.pTarget = pTarget;
				tInfo.vPos = *(_float3*)&pTarget->Get_World().m[3][0];
				if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
					return;
				if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit2"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
					return;
				dynamic_cast<CCamera_Dynamic*>(pInstance->Find_Layer(m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Camera"))->Get_Objects().front())->CriHit();
			}
			else
			{
				pTarget->Set_Hp(m_tInfo.iDmg);
				pTarget->Set_Hit(m_tInfo.iDmg, Get_CollisionPos(pTarget, this));
			}
			if (pTarget->Get_Info().iHp <= 0)
				pTarget->Set_Dead();
			Set_Dead();
		}
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_TOTEM, TEXT("Com_Collider"), &pTarget))
	{

		_float fCri = _float(rand() % 100 + 1);
		_float fLUK = (_float)dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iLUK / 2.f;

		if (fCri <= fLUK)
		{
			pTarget->Set_Hp(m_tInfo.iDmg * 2);
			pTarget->Set_Hit(m_tInfo.iDmg * 2, *(_float3*)&pTarget->Get_World().m[3][0]);
			CGameObject::INFO tInfo;
			tInfo.pTarget = pTarget;
			tInfo.vPos = *(_float3*)&pTarget->Get_World().m[3][0];
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit2"), m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			dynamic_cast<CCamera_Dynamic*>(pInstance->Find_Layer(m_tInfo.pTarget->Get_Info().iLevelIndex, TEXT("Layer_Camera"))->Get_Objects().front())->CriHit();
		}
		else
		{
			pTarget->Set_Hp(m_tInfo.iDmg);
			pTarget->Set_Hit(m_tInfo.iDmg, Get_CollisionPos(pTarget, this));
		}
		if (pTarget->Get_Info().iHp <= 0)
			pTarget->Set_Dead();
		Set_Dead();
	}
	Safe_Release(pInstance);
}
_float3 CThunderSword::Get_CollisionPos(CGameObject * pDest, CGameObject * pSour)
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
HRESULT CThunderSword::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (FAILED(m_pTextureCom->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CThunderSword::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CThunderSword::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
HRESULT CThunderSword::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Components(TEXT("Com_Texture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_ThunderSword"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.0f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CThunderSword::SetUp_RenderState()
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

HRESULT CThunderSword::Release_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

CThunderSword * CThunderSword::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CThunderSword*	pInstance = new CThunderSword(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CThunderSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CThunderSword::Clone(void* pArg)
{
	CThunderSword*	pInstance = new CThunderSword(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CThunderSword"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CThunderSword::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CThunderSword::Free()
{
	__super::Free();
	
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
}
