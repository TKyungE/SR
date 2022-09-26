#include "stdafx.h"
#include "..\Public\Dandelion.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "Layer.h"
#include "Camera_Dynamic.h"
#include "QuestManager.h"

CDandelion::CDandelion(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CDandelion::CDandelion(const CDandelion& rhs)
	: CGameObject(rhs)
{
}
HRESULT CDandelion::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDandelion::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	memcpy(&m_tInfo, pArg, sizeof(INFO));
	if (FAILED(SetUp_Components()))
		return E_FAIL;


	//m_tInfo.vPos.y += 0.f;
	_float3 vScale = { 1.f,1.f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 6;
	m_tFrame.fFrameSpeed = 0.15f;
	m_tInfo.bDead = false;
	m_tInfo.iDmg = 66;
	m_tInfo.fX = 0.5f;
	m_tInfo.iMaxHp = 15000;
	m_tInfo.iHp = m_tInfo.iMaxHp;
	m_tInfo.iMp = 1;
	m_tInfo.iExp = 20;
	m_tInfo.iMonsterType = (_int)MON_DANDELION;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 1.f,0.5f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), m_tInfo.iLevelIndex, TEXT("Layer_Status"), &tInfo);

	tInfo.vPos = { 0.7f,0.7f,1.f };

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
	m_StatInfo = pGameInstance->Find_Layer(LEVEL_STATIC, TEXT("Layer_StatInfo"))->Get_Objects().front();
	Safe_Release(pGameInstance);


	return S_OK;
}

void CDandelion::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (!m_bRespawn)
	{
		m_fSkillCool += fTimeDelta;
		m_fCollTime += fTimeDelta;
		if (m_tInfo.iMp == 2 && !m_bAngry)
		{
			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			CGameObject::INFO tInfo;
			tInfo.pTarget = this;
			pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Angry"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo);
			Safe_Release(pGameInstance);
			m_bAngry = true;
		}
		OnTerrain();
		if (!m_bDead)
			Check_Front();
		if (m_eCurState == DEAD)
		{
			if (m_tFrame.iFrameStart == 4)
			{
				m_fDeadTime += fTimeDelta;
				if (m_fDeadTime > 3.f)
				{
					_float3 vDeadPos = { -50000.f,-50000.f,-50000.f };
					m_pTransformCom->Set_State(CTransform::STATE_POSITION, vDeadPos);
					m_pTransformCom->Bind_OnGraphicDev();
					m_bRespawn = true;
					return;
				}
			}
			if (m_tFrame.iFrameStart != 4)
				Move_Frame(fTimeDelta);
			m_tInfo.bDead = false;
			return;
		}
		if (m_tInfo.iMp == 1)
		{
			if (!m_bSkill && !m_bDead && !m_bRun)
				Chase(fTimeDelta);

			if (m_bRun)
				Chase2(fTimeDelta);
		}
		else if (!m_bSkill && !m_bDead)
			Chase3(fTimeDelta);

		if (m_tInfo.iMp == 1 && !m_bIDLE)
		{
			MonsterMove(fTimeDelta);
		}


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
			ERR_MSG(TEXT("Failed to Add CollisionGroup : CDandelion"));
			return;
		}

		Safe_Release(pInstance);
	}
	else
	{
		m_fRespawnTime += fTimeDelta;
		if (m_fRespawnTime > 10.f)
		{
			RespawnMonster();
			m_fRespawnTime = 0.f;
			m_bRespawn = false;
		}
	}

	m_tInfo.bDead = false;
}

void CDandelion::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (!m_bRespawn)
	{
		if (!m_bDead)
		{
			Check_Hit();
			Motion_Change();
			CheckColl();
		}
		OnBillboard();
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
}

HRESULT CDandelion::Render(void)
{
	if (!m_bRespawn)
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
	}
	return S_OK;
}
HRESULT CDandelion::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_IDLE_Front"), (CComponent**)&m_pTextureComIDLE_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_IDLE_Back"), (CComponent**)&m_pTextureComIDLE_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Move_Front"), (CComponent**)&m_pTextureComMove_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Move_Back"), (CComponent**)&m_pTextureComMove_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Attack_Front"), (CComponent**)&m_pTextureComAttack_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Attack_Back"), (CComponent**)&m_pTextureComAttack_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Dead_Front"), (CComponent**)&m_pTextureComDead_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dandelion_Dead_Back"), (CComponent**)&m_pTextureComDead_Back)))
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

HRESULT CDandelion::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CDandelion::Release_RenderState(void)
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CDandelion::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);;
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), m_tInfo.iLevelIndex, TEXT("Layer_DmgFont"), &tInfo);
		tInfo.vPos = m_tInfo.vTargetPos;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;
		Safe_Release(pGameInstance);
	}
}

void CDandelion::Chase(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (Distance >= 5.f)
		m_bIDLE = false;
	else
		m_bIDLE = true;
	if (1.f >= Distance)
	{
		if (m_fSkillCool >	0.4f)
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
	else if (1.f < Distance && 5.f > Distance)
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
		//	vPosition.y = vTargetPos.y += 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
}
void CDandelion::Chase2(_float fTimeDelta)
{
	_float MinDist = 100000.f;
	_float3 MinTarget;
	_float HelpDistance = 0.f;
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Monster")) != nullptr)
	{
		for (auto& iter : pGameInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Monster"))->Get_Objects())
		{
			_float3 Target = *(_float3*)&iter->Get_World().m[3][0];
			if (iter->Get_Info().iHp <= 0)
				continue;
			HelpDistance = D3DXVec3Length(&(Target - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			if (HelpDistance <= 0.f)
				continue;
			if (HelpDistance <= 2.f)
			{
				iter->Set_Mp(1);
				m_tInfo.iMp = 2;
			}
			if (MinDist > HelpDistance)
			{
				MinDist = HelpDistance;
				MinTarget = *(_float3*)&iter->Get_World().m[3][0];
			}
		}

		if (2.f <= MinDist)
		{
			if (!m_bSkill)
				m_eCurState = MOVE;
			if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
				m_tFrame.iFrameStart = m_tFrame.iFrameEnd;


			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			vPosition += *D3DXVec3Normalize(&MinTarget, &(MinTarget - vPosition)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;
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
			//	vPosition.y = vTargetPos.y += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		if (10.f > Distance)
		{
			if (!m_bSkill)
				m_eCurState = MOVE;
			if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
				m_tFrame.iFrameStart = m_tFrame.iFrameEnd;


			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
			vPosition -= *D3DXVec3Normalize(&MinTarget, &(MinTarget - vPosition)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;
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
			//	vPosition.y = vTargetPos.y += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	Safe_Release(pGameInstance);
}
void CDandelion::Chase3(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (1.f >= Distance)
	{
		if (m_fSkillCool >	0.3f)
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
	else if (1.f < Distance && 10000.f > Distance)
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
		//	vPosition.y = vTargetPos.y += 2.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
}
void CDandelion::OnTerrain()
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

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

CDandelion * CDandelion::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CDandelion* pInstance = new CDandelion(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CDandelion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDandelion::Clone(void * pArg)
{
	CDandelion* pInstance = new CDandelion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CDandelion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CDandelion::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CDandelion::Free(void)
{
	__super::Free();
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureComIDLE_Front);
	Safe_Release(m_pTextureComIDLE_Back);
	Safe_Release(m_pTextureComMove_Front);
	Safe_Release(m_pTextureComMove_Back);
	Safe_Release(m_pTextureComAttack_Front);
	Safe_Release(m_pTextureComAttack_Back);
	Safe_Release(m_pTextureComDead_Front);
	Safe_Release(m_pTextureComDead_Back);
}



HRESULT CDandelion::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CDandelion::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CDandelion::Skill_DefaultAttack(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.iLevelIndex = LEVEL_GAMEPLAY;
	tInfo.vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
	

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DandelionAttack"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
		return E_FAIL;
	CSoundMgr::Get_Instance()->PlayEffect(L"Dande_Attack.wav", fSOUND);
	Safe_Release(pGameInstance);

	return S_OK;
}
void CDandelion::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 6;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		case MOVE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 7;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		case DEAD:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 4;
			m_tFrame.fFrameSpeed = 0.25f;
			m_bDead = true;
			break;
		case SKILL:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 4;
			m_tFrame.fFrameSpeed = 0.2f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CDandelion::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComIDLE_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComIDLE_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case MOVE:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComMove_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComMove_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case DEAD:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComDead_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComDead_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case SKILL:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComAttack_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComAttack_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
}
void CDandelion::Check_Front()
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

	if (vTargetPos.z > vPos.z)
		m_bFront = false;
	if (vTargetPos.z <= vPos.z)
		m_bFront = true;
	if (vTargetPos.x > vPos.x)
		m_bRight = true;
	if (vTargetPos.x <= vPos.x)
		m_bRight = false;

	if (m_tInfo.bDead && m_eCurState != DEAD)
	{
		DropItem();
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

		CSoundMgr::Get_Instance()->PlayEffect(L"Dande_Die.wav", fSOUND);
	}
	if ((((float)m_tInfo.iHp / (float)m_tInfo.iMaxHp) < 0.3f) && !m_bRun)
	{
		m_bRun = true;
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Help"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo);
		Safe_Release(pGameInstance);
	}
}
void CDandelion::Use_Skill(_float fTimeDelta)
{
	if (!m_bSkill && m_tFrame.iFrameStart == 2)
	{
		Skill_DefaultAttack(TEXT("Layer_MonsterSkill"));
		m_bSkill = true;
	}
	if (m_tFrame.iFrameStart == 4)
	{
		m_eCurState = IDLE;
		m_tFrame.iFrameStart = 0;
		m_bSkill = false;
	}
}
HRESULT CDandelion::TextureRender()
{
	switch (m_eCurState)
	{
	case IDLE:
		if (m_bFront)
		{
			if (FAILED(m_pTextureComIDLE_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureComIDLE_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		break;
	case MOVE:
		if (m_bFront)
		{
			if (FAILED(m_pTextureComMove_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureComMove_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		break;
	case DEAD:
		if (m_bFront)
		{
			if (FAILED(m_pTextureComDead_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureComDead_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		break;
	case SKILL:
		if (m_bFront)
		{
			if (FAILED(m_pTextureComAttack_Front->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureComAttack_Back->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		}
		break;
	default:
		break;
	}
	return S_OK;
}
void CDandelion::MonsterMove(_float fTimeDelta)
{

	m_fMove += fTimeDelta;
	if (m_fMove > 1.5f)
	{
		m_irand = rand() % 4;
		m_fMove = 0.f;
	}
	_float3 vPos;
	switch (m_irand)
	{
	case 0:
		m_pTransformCom->Go_Straight(fTimeDelta);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_eCurState = MOVE;
		if (vPos.z > m_tInfo.vPos.z + 3.f)
		{
			m_bFront = false;
			m_pTransformCom->Go_Backward(fTimeDelta);
			m_eCurState = IDLE;
		}
		break;
	case 1:
		m_pTransformCom->Go_Backward(fTimeDelta);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_eCurState = MOVE;
		if (vPos.z < m_tInfo.vPos.z - 3.f)
		{
			m_bFront = true;
			m_pTransformCom->Go_Straight(fTimeDelta);
			m_eCurState = IDLE;
		}
		break;
	case 2:
		m_pTransformCom->Go_Left(fTimeDelta);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_eCurState = MOVE;
		if (vPos.x < m_tInfo.vPos.x - 3.f)
		{
			m_pTransformCom->Go_Right(fTimeDelta);
			m_eCurState = IDLE;
			m_bRight = false;
		}
		break;
	case 3:
		m_pTransformCom->Go_Right(fTimeDelta);
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_eCurState = MOVE;
		if (vPos.x > m_tInfo.vPos.x + 3.f)
		{
			m_pTransformCom->Go_Left(fTimeDelta);
			m_eCurState = IDLE;
			m_bRight = true;
		}
		break;
	default:
		break;
	}




}
HRESULT CDandelion::RespawnMonster()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);
	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 6;
	m_tFrame.fFrameSpeed = 0.15f;
	m_tInfo.iHp = m_tInfo.iMaxHp;
	m_tInfo.iMp = 1;
	m_bFront = false;
	m_fSkillCool = 0.f;
	m_fRespawnTime = 0.f;
	m_fDeadTime = 0.f;
	m_fMove = 0.f;
	m_irand = 0;
	m_bSkill = false;
	m_bMove = false;
	m_bDead = false;
	m_bRun = false;
	m_bIDLE = false;
	m_bRespawn = false;
	m_bAngry = false;
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 1.f,0.5f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), m_tInfo.iLevelIndex, TEXT("Layer_Status"), &tInfo);
	tInfo.vPos = { 0.7f,0.7f,1.f };
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo);
	Safe_Release(pGameInstance);
	return S_OK;
}
void CDandelion::CheckColl()
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

	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_OBJECT, TEXT("Com_Collider"), &pTarget))
	{
		_float3 vBackPos;
		if (fabs(pInstance->Get_Collision().x) < fabs(pInstance->Get_Collision().z))
		{
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x - pInstance->Get_Collision().x;
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z;
		}
		else if (fabs(pInstance->Get_Collision().z) < fabs(pInstance->Get_Collision().x))
		{
			vBackPos.z = m_pTransformCom->Get_State(CTransform::STATE_POSITION).z - pInstance->Get_Collision().z;
			vBackPos.x = m_pTransformCom->Get_State(CTransform::STATE_POSITION).x;
		}
		vBackPos.y = m_pTransformCom->Get_State(CTransform::STATE_POSITION).y;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBackPos);
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYERSKILL, TEXT("Com_Collider"), &pTarget) && m_fCollTime > 0.1f)
	{
		_float fCri = _float(rand() % 100 + 1);
		_float fLUK = (_float)dynamic_cast<CStatInfo*>(m_StatInfo)->Get_Stat().iLUK / 2.f;

		if (fCri <= fLUK)
		{
			Set_Hp(pTarget->Get_Info().iDmg * 2);
			Set_Hit(pTarget->Get_Info().iDmg * 2, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CGameObject::INFO tInfo;
			tInfo.pTarget = this;
			tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_CriHit2"), m_tInfo.iLevelIndex, TEXT("Layer_Effect"), &tInfo)))
				return;
			dynamic_cast<CCamera_Dynamic*>(pInstance->Find_Layer(m_tInfo.iLevelIndex, TEXT("Layer_Camera"))->Get_Objects().front())->CriHit();
		}
		else
		{
			Set_Hp(pTarget->Get_Info().iDmg);
			Set_Hit(pTarget->Get_Info().iDmg, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		if (m_tInfo.iHp <= 0)
			Set_Dead();

		m_fCollTime = 0.f;
	}
	Safe_Release(pInstance);
}
void CDandelion::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale;
	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	if (m_bRight && m_bFront || m_bRight && !m_bFront)
	{
		m_pTransformCom->Set_Scaled(_float3(-1.f, 1.f, 1.f));
		vRight.x = -1;
	}
	else if (!m_bRight && !m_bFront || !m_bRight && m_bFront)
		m_pTransformCom->Set_Scaled(_float3(1.f, 1.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
void CDandelion::DropItem()
{
	_int iDest = rand() % 2;
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.pTerrain = m_tInfo.pTarget;
	if (iDest == 0)
		tInfo.iLv = 2;
	else
		tInfo.iLv = 22;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Item"), &tInfo);

	Safe_Release(pGameInstance);
}