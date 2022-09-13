#include "stdafx.h"
#include "..\Public\FireDragon.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "SoundMgr.h"

CFireDragon::CFireDragon(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CFireDragon::CFireDragon(const CFireDragon& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFireDragon::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragon_IDLE_Front"), (CComponent**)&m_pTextureComIDLE_Front)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragon_IDLE_Back"), (CComponent**)&m_pTextureComIDLE_Back)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragon_Attack"), (CComponent**)&m_pTextureComAttack)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireDragon_Dead"), (CComponent**)&m_pTextureComDead)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 2.f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireDragon::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CFireDragon::Release_RenderState(void)
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);
	return S_OK;
}

void CFireDragon::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);;
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), LEVEL_GAMEPLAY, TEXT("Layer_DmgFont"), &tInfo);
		tInfo.vPos = m_tInfo.vTargetPos;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo);
		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;
		Safe_Release(pGameInstance);
	}
}

void CFireDragon::Chase(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		
	if (0.25f < Distance && m_eCurState != DEAD)
		{
			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

			vPosition += *D3DXVec3Normalize(&vTargetPos, &(vTargetPos - vPosition)) * m_pTransformCom->Get_TransformDesc().fSpeedPerSec * fTimeDelta;
			vPosition.y += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	else
		{
			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
			if(m_eCurState == DEAD)
				vPosition.y = vTargetPos.y += 2.7f;
			else
				vPosition.y = vTargetPos.y += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
}
void CFireDragon::OnTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

CFireDragon * CFireDragon::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CFireDragon* pInstance = new CFireDragon(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CFireDragon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFireDragon::Clone(void * pArg)
{
	CFireDragon* pInstance = new CFireDragon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CFireDragon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CFireDragon::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CFireDragon::Free(void)
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureComIDLE_Front);
	Safe_Release(m_pTextureComIDLE_Back);
	Safe_Release(m_pTextureComDead);
	Safe_Release(m_pTextureComAttack);
}

HRESULT CFireDragon::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFireDragon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	memcpy(&m_tInfo, pArg, sizeof(INFO));
	m_tInfo.vPos.y += 2.f;
	_float3 vScale = { 8.f,8.f,1.f };
	m_pTransformCom->Set_Scaled(vScale);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_tInfo.vPos);

	m_ePreState = STATE_END;
	m_eCurState = IDLE;
	m_tFrame.iFrameStart = 0;
	m_tFrame.iFrameEnd = 6;
	m_tFrame.fFrameSpeed = 0.1f;
	m_tInfo.bDead = false;
	m_tInfo.iDmg = 66;
	m_tInfo.iMoney = 4444;
	m_tInfo.fX = 2.f;
	m_tInfo.iMaxHp = 999999;
	m_tInfo.iHp = m_tInfo.iMaxHp;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), LEVEL_GAMEPLAY, TEXT("Layer_Status"), &tInfo);
	tInfo.vPos = { 3.f,3.f,1.f };

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo);

	Safe_Release(pGameInstance);

	
	return S_OK;
}

void CFireDragon::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	
	OnTerrain();
	Check_Front();
	if (m_eCurState == DEAD)
	{
		if (m_tFrame.iFrameStart == 3)
		{
			m_bDead = true;
			m_fDeadTime += fTimeDelta;
			if (m_fDeadTime > 5.f)
			{
				m_tInfo.bDead = true;
				return;
			}
		}
	}
	if (!m_bDead)
	{
		Move_Frame(fTimeDelta);
		Use_Skill(fTimeDelta);
		Create_Wind();
	}

	if (nullptr != m_tInfo.pTarget)
		Chase(fTimeDelta);

	m_tInfo.bDead = false;

	if (g_bCollider)
		m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 1.f);
}

void CFireDragon::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
	if (!m_bDead)
	{
		Check_Hit();
		Motion_Change();
	}
	OnBillboard();

	CGameInstance* pInstance = CGameInstance::Get_Instance();

	Safe_AddRef(pInstance);

	if (pInstance->IsInFrustum(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pTransformCom->Get_Scale()))
	{
		if (nullptr != m_pRendererCom)
			m_pRendererCom->Add_RenderGroup_Front(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Safe_Release(pInstance);
}

HRESULT CFireDragon::Render(void)
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

	m_pColliderCom->Render();

	return S_OK;
}

HRESULT CFireDragon::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CFireDragon::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}
HRESULT CFireDragon::Skill_FireSpear(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;

	for (int i = 0; i < 100; ++i)
	{
		_float iSour = rand() % 4000 * 0.001f;
		_float iTemp = rand() % 4000 * 0.001f;
		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y;
		tInfo.vPos.z = vPos.z + iTemp;
		tInfo.iLevelIndex = LEVEL_GAMEPLAY;

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_FireSpear"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}
HRESULT CFireDragon::Skill_Meteor(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	for (int i = 0; i < 100; ++i)
	{
		_float iSour = rand() % 4000 * 0.001f;
		_float iTemp = rand() % 4000 * 0.001f;

		_float3 vPos = { 0.f,0.f,0.f };
		tInfo.vPos.x = vPos.x + iSour;
		tInfo.vPos.y = vPos.y;
		tInfo.vPos.z = vPos.z + iTemp;
		tInfo.iLevelIndex = LEVEL_GAMEPLAY;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Meteor"), LEVEL_GAMEPLAY, pLayerTag, &tInfo)))
			return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}
void CFireDragon::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 6;
			m_tFrame.fFrameSpeed = 0.1f;
			break;
		case DEAD:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 3;
			m_tFrame.fFrameSpeed = 0.5f;
			break;
		case SKILL:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 8;
			m_tFrame.fFrameSpeed = 0.05f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CFireDragon::Move_Frame(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	case IDLE:
		if (m_bFront)
			m_tFrame.iFrameStart = m_pTextureComIDLE_Front->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		else
			m_tFrame.iFrameStart = m_pTextureComIDLE_Back->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case DEAD:
			m_tFrame.iFrameStart = m_pTextureComDead->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	case SKILL:
			m_tFrame.iFrameStart = m_pTextureComAttack->MoveFrame(fTimeDelta, m_tFrame.fFrameSpeed, m_tFrame.iFrameEnd);
		break;
	default:
		break;
	}
}
void CFireDragon::Check_Front()
{
	_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];

	if (vTargetPos.z > vPos.z)
		m_bFront = false;
	if (vTargetPos.z <= vPos.z)
		m_bFront = true;

	if (m_eCurState == SKILL)
	{
		if (m_tFrame.iFrameStart == 8)
		{
			m_eCurState = IDLE;
			m_tFrame.iFrameStart = 0;
		}
	}
	if (m_tInfo.bDead && m_eCurState != DEAD)
	{
		m_eCurState = DEAD;
		m_tFrame.iFrameStart = 0;
	}

}
void CFireDragon::Use_Skill(_float fTimeDelta)
{
	m_fMeteorCool += fTimeDelta;
	m_fFireSpearCool += fTimeDelta;

	if (m_fFireSpearCool > 10.f && m_eCurState != SKILL)
	{
		Skill_FireSpear(TEXT("Layer_MonsterSkill"));
		m_fFireSpearCool = 0.f;
		m_eCurState = SKILL;
		m_tFrame.iFrameStart = 0;
	}
	if (m_fMeteorCool > 9.f && m_eCurState != SKILL)
	{
		Skill_Meteor(TEXT("Layer_MonsterSkill"));
		m_fMeteorCool = 0.f;
		m_eCurState = SKILL;
		m_tFrame.iFrameStart = 0;
	}
}
HRESULT CFireDragon::Create_Wind()
{
	if (m_eCurState == IDLE)
	{
		if (m_tFrame.iFrameStart == 3)
		{
			CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);
			CGameObject::INFO tInfo;

			_float3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			tInfo.vPos = vPos;
			tInfo.pTarget = this;
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Wind"), LEVEL_GAMEPLAY, TEXT("Layer_Effect"), &tInfo)))
				return E_FAIL;
			Safe_Release(pGameInstance);
		}
	}
	return S_OK;
}
HRESULT CFireDragon::TextureRender()
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
	case DEAD:
			if (FAILED(m_pTextureComDead->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		break;
	case SKILL:
			if (FAILED(m_pTextureComAttack->Bind_OnGraphicDev(m_tFrame.iFrameStart)))
				return E_FAIL;
		break;
	default:
		break;
	}
	return S_OK;
}
void CFireDragon::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale = { 8.f,8.f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *(_float3*)&ViewMatrix.m[0][0] * vScale.x);
//	m_pTransformCom->Set_State(CTransform::STATE_UP, *(_float3*)&ViewMatrix.m[1][0]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}