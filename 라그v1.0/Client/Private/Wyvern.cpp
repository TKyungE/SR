#include "stdafx.h"
#include "..\Public\Wyvern.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "Layer.h"
#include "QuestManager.h"

CWyvern::CWyvern(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CWyvern::CWyvern(const CWyvern& rhs)
	: CGameObject(rhs)
{
}
HRESULT CWyvern::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWyvern::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	_float3 vScale = { 1.5f,1.5f,1.5f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 7;
	m_tFrame.fFrameSpeed = 0.15f;
	m_tInfo.bDead = false;
	m_tInfo.iDmg = 66;
	m_tInfo.fX = 0.5f;
	m_tInfo.iMaxHp = 5000;
	m_tInfo.iHp = m_tInfo.iMaxHp;
	m_tInfo.iMp = 1;
	m_tInfo.iExp = 40;
	m_tInfo.iMonsterType = MON_WYVERN;
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 1.f,0.5f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), m_tInfo.iLevelIndex, TEXT("Layer_Status"), &tInfo);

	Safe_Release(pGameInstance);


	return S_OK;
}

void CWyvern::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fSkillCool += fTimeDelta;
	

	if (!m_bDead)
		Check_Front();

	if (m_eCurState == DEAD)
	{
		if (m_tFrame.iFrameStart == 2)
			{
				m_tInfo.bDead = true;
					return;
			}
		if (m_tFrame.iFrameStart != 2)
			Move_Frame(fTimeDelta);
		m_tInfo.bDead = false;
			return;
	}
		
		if (!m_bSkill && !m_bDead)
			Chase(fTimeDelta);


		Move_Frame(fTimeDelta);
		if (m_eCurState == SKILL)
			Use_Skill(fTimeDelta);

		m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);

		CGameInstance* pInstance = CGameInstance::Get_Instance();
		if (nullptr == pInstance)
			return;

		Safe_AddRef(pInstance);

		if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_MONSTER, this)))
		{
			ERR_MSG(TEXT("Failed to Add CollisionGroup : CWyvern"));
			return;
		}

		Safe_Release(pInstance);
	

	m_tInfo.bDead = false;
}

void CWyvern::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!m_bDead)
	{
		Check_Hit();
		Motion_Change();
		CheckColl();
	}
	OnBillboard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_NONALPHABLEND, this);
	}

	Safe_Release(pInstance);
}


HRESULT CWyvern::Render(void)
{
	
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

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
HRESULT CWyvern::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wyvern_IDLE_Front"), (CComponent**)&m_pTextureComIDLE_Front)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wyvern_IDLE_Front"), (CComponent**)&m_pTextureComMove_Front)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wyvern_Attack_Front"), (CComponent**)&m_pTextureComAttack_Front)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Wyvern_Dead_Front"), (CComponent**)&m_pTextureComDead_Front)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;
	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 1.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWyvern::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CWyvern::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

void CWyvern::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		if (nullptr == pGameInstance)
			return;

		Safe_AddRef(pGameInstance);

		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);;
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), LEVEL_SKY, TEXT("Layer_DmgFont"), &tInfo)))
			return;

		tInfo.vPos = m_tInfo.vTargetPos;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), LEVEL_SKY, TEXT("Layer_Effect"), &tInfo)))
			return;

		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;

		Safe_Release(pGameInstance);
	}
}

void CWyvern::Chase(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (Distance >= 50.f)
		m_bIDLE = false;
	else
		m_bIDLE = true;
	if (8.f >= Distance)
	{
		if (m_fSkillCool >	3.f)
		{
			m_fSkillCool = 0.f;
			m_eCurState = SKILL;
			m_tFrame.iFrameStart = 0;
		}
		if (m_eCurState != SKILL)
			m_eCurState = IDLE;
		if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
			m_tFrame.iFrameStart = m_tFrame.iFrameEnd;
		_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		//	vPosition.y = vTargetPos.y += 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else if (8.f < Distance && 50.f > Distance)
	{
		if (!m_bSkill)
			m_eCurState = MOVE;
		if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
			m_tFrame.iFrameStart = m_tFrame.iFrameEnd;
		_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

		vPosition += *D3DXVec3Normalize(&vTargetPos, &(vTargetPos - vPosition)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;
		//	vPosition.y += 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
	else
	{
		if (!m_bSkill)
			m_eCurState = IDLE;
		if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
			m_tFrame.iFrameStart = m_tFrame.iFrameEnd;
		_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
}

CWyvern * CWyvern::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CWyvern* pInstance = new CWyvern(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CWyvern"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWyvern::Clone(void * pArg)
{
	CWyvern* pInstance = new CWyvern(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CWyvern"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CWyvern::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CWyvern::Free(void)
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureComIDLE_Front);
	Safe_Release(m_pTextureComMove_Front);
	Safe_Release(m_pTextureComAttack_Front);
	Safe_Release(m_pTextureComDead_Front);

}



HRESULT CWyvern::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CWyvern::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CWyvern::Skill_PoisonArrow(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;

	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.iLevelIndex = LEVEL_SKY;
	tInfo.vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_ShadowBall"), LEVEL_SKY, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
void CWyvern::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 7;
			m_tFrame.fFrameSpeed = 0.15;
			break;
		case MOVE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 7;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		case DEAD:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 2;
			m_tFrame.fFrameSpeed = 0.3f;
			m_bDead = true;
			break;
		case SKILL:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 7;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CWyvern::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		m_tFrame.iFrameStart = m_pTextureComIDLE_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case MOVE:
		m_tFrame.iFrameStart = m_pTextureComMove_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case DEAD:
		m_tFrame.iFrameStart = m_pTextureComDead_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case SKILL:
		m_tFrame.iFrameStart = m_pTextureComAttack_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
}
void CWyvern::Check_Front()
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

	
	if (vTargetPos.x > vPos.x)
		m_bRight = true;
	if (vTargetPos.x <= vPos.x)
		m_bRight = false;

	if (m_tInfo.bDead && m_eCurState != DEAD)
	{
		m_tInfo.pTarget->Set_Exp(m_tInfo.iExp);
		m_eCurState = DEAD;
		m_tFrame.iFrameStart = 0;
		m_bDead = true;
		Motion_Change();
		CQuestManager* pQuestManager = CQuestManager::Get_Instance();
		if (nullptr == pQuestManager)
			return;

		Safe_AddRef(pQuestManager);

		pQuestManager->Increase_Count((MONSTERTYPE)m_tInfo.iMonsterType);

		Safe_Release(pQuestManager);
	}
}
void CWyvern::Use_Skill(_float fTimeDelta)
{
	if (!m_bSkill && m_tFrame.iFrameStart == 4)
	{
		Skill_PoisonArrow(TEXT("Layer_MonsterSkill"));
		m_bSkill = true;
	}
	if (m_tFrame.iFrameStart == 5)
	{
		m_eCurState = IDLE;
		m_tFrame.iFrameStart = 0;
		m_bSkill = false;
	}
}
HRESULT CWyvern::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (FAILED(m_pTextureComIDLE_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	case MOVE:
		if (FAILED(m_pTextureComMove_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	case DEAD:
		if (FAILED(m_pTextureComDead_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	case SKILL:
		if (FAILED(m_pTextureComAttack_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
			return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}

void CWyvern::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale;
	if (m_bRight)
	 vScale = { -1.5f,1.5f,1.5f };
	else if (!m_bRight)
	 vScale = { 1.5f,1.5f,1.5f };


	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0] * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
void CWyvern::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	CGameObject* pTarget;
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_MONSTER, TEXT("Com_Collider"), &pTarget))
	{
		_float3 vBackPos;

		if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
		{
			if (pInstance->Get_Collision().x > 0)
			{
				vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
				vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
				vBackPos.z -= 0.01f;
			}
			else if (pInstance->Get_Collision().x < 0)
			{
				vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
				vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
				vBackPos.z += 0.01f;
			}
		}
		else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
		{
			if (pInstance->Get_Collision().z > 0)
			{
				vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
				vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
				vBackPos.x -= 0.01f;
			}
			else if (pInstance->Get_Collision().z < 0)
			{
				vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
				vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
				vBackPos.x += 0.01f;
			}
		}

		vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	}



	Safe_Release(pInstance);
}
