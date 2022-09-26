#include "stdafx.h"
#include "..\Public\Maiden.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "SoundMgr.h"
#include "Layer.h"
#include "TextBox.h"

CMaiden::CMaiden(LPDIRECT3DDEVICE9 _pGraphic_Device)
	: CGameObject(_pGraphic_Device)
{
}

CMaiden::CMaiden(const CMaiden& rhs)
	: CGameObject(rhs)
{
}
HRESULT CMaiden::Initialize_Prototype(void)
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaiden::Initialize(void * pArg)
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
	m_tFrame.iFrameEnd = 5;
	m_tFrame.fFrameSpeed = 0.15f;
	m_tInfo.bDead = false;
	m_tInfo.iDmg = 66;
	m_tInfo.fX = 0.5f;
	m_tInfo.iMaxHp = 9999;
	m_tInfo.iHp = m_tInfo.iMaxHp;
	m_tInfo.iMp = 0;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = { 1.f,0.7f,1.f };
	tInfo.iLevelIndex = m_tInfo.iLevelIndex;
	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_WorldHpBar"), LEVEL_MIDBOSS, TEXT("Layer_Status"), &tInfo);
	tInfo.vPos = { 1.f,1.f,1.f };

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Shadow"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo);

	Safe_Release(pGameInstance);

	D3DXMatrixOrthoLH(&m_ProjMatrix, (_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f);

	m_fSizeX = 600.f;
	m_fSizeY = 600.f;
	m_fCharX = g_iWinSizeX * 0.5f;
	m_fCharY = g_iWinSizeY * 0.5f;

	m_pCharTransformCom->Set_Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pCharTransformCom->Set_State(CTransform::STATE_POSITION, _float3(m_fCharX - g_iWinSizeX * 0.5f, -m_fCharY + g_iWinSizeY * 0.5f, 0.f));

	Ready_Script();

	g_iCut = 40;

	return S_OK;
}

void CMaiden::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	OnTerrain();

	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);

	if (40 == g_iCut && !m_bTalk)
	{
		m_bTalk = true;

		CTextBox::TINFO tTInfo;
		tTInfo.iScriptSize = (_int)m_vScript.size();

		tTInfo.pScript = new wstring[m_vScript.size()];
		for (_int i = 0; i < m_vScript.size(); ++i)
			tTInfo.pScript[i] = m_vScript[i];

		tTInfo.iQuestIndex = 3;
		tTInfo.iLevelIndex = m_tInfo.iLevelIndex;
		tTInfo.iNumQuest = 40;

		if (FAILED(pInstance->Add_GameObject(TEXT("Prototype_GameObject_TextBox"), m_tInfo.iLevelIndex, TEXT("Layer_UI"), &tTInfo)))
			return;

		if (40 == g_iReward)
		{
			g_iQuest = 0;
			g_iReward = 0;
		}
	}
	else
	{
		if (!m_bDead)
		{
			Check_Front();
			if (m_bBlueFire)
				Create_BlueFire(fTimeDelta);
		}
		if (m_eCurState == DEAD)
		{
			if (m_tFrame.iFrameStart == 4)
			{
				Set_Dead();
				return;

			}
			if (m_tFrame.iFrameStart != 4)
				Move_Frame(fTimeDelta);
			m_tInfo.bDead = false;
			return;
		}
		if (!m_bLastHeal)
		{
			m_fSkillCool += fTimeDelta;
			if (!m_bSkill && !m_bDead && !m_bSkill2)
				Chase(fTimeDelta);
			if (!m_bSkill)
				m_fSkillCool2 += fTimeDelta;

			if (m_fSkillCool2 > 13.f)
			{
				m_bSkill2 = true;
				m_fSkillCool2 = 0;
				m_fSkillCool = 0;
			}

		}
		Move_Frame(fTimeDelta);
		if (m_eCurState == SKILL && !m_bSkill2)
			Use_Skill(fTimeDelta);

		if (m_bSkill2 && m_bStart && !m_bSkill)
			Use_Skill2(fTimeDelta);

		m_pColliderCom->Set_Transform(m_pTransformCom->Get_WorldMatrix(), 0.5f);

		if (FAILED(pInstance->Add_ColiisionGroup(COLLISION_BOSS, this)))
		{
			ERR_MSG(TEXT("Failed to Add CollisionGroup : CMaiden"));
			return;
		}

		m_tInfo.bDead = false;
	}

	m_fAlpha += 0.006f;

	if (0.2f < m_fTalkFrame)
	{
		++m_iTalkFrame;
		m_fTalkFrame = 0.f;

		if (8 < m_iTalkFrame)
			m_iTalkFrame = 0;
	}
	m_fTalkFrame += fTimeDelta;

	Safe_Release(pInstance);
}

void CMaiden::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (!m_bDead)
	{
		Check_Hit();
		Motion_Change();
		if (m_bSkill2)
		{
			m_CollTime += fTimeDelta;
			if (m_CollTime > 0.04f)
			{
				CheckColl();
				m_CollTime = 0.f;
			}
		}
	}
	OnBillboard();
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CMaiden::Render(void)
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	Off_SamplerState();

	if (0 != g_iCut && m_bTalk)
	{
		if (FAILED(m_pCharTransformCom->Bind_OnGraphicDev()))
			return E_FAIL;

		_float4x4	WorldMatrix, ViewMatrix;

		WorldMatrix = *D3DXMatrixTranspose(&WorldMatrix, &m_pCharTransformCom->Get_WorldMatrix());

		D3DXMatrixIdentity(&ViewMatrix);

		_float4x4 SaveViewMatrix, SaveProjVatrix;

		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &SaveViewMatrix);
		m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &SaveProjVatrix);

		/*	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &ViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);*/

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", D3DXMatrixTranspose(&m_ProjMatrix, &m_ProjMatrix), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

		m_pShaderCom->Set_Texture("g_Texture", m_pCharTextureCom->Get_Texture(m_iTalkFrame));

		/*if (FAILED(m_pCharTextureCom->Bind_OnGraphicDev(0)))
		return E_FAIL;*/

		m_pShaderCom->Begin(1);

		m_pCharVIBufferCom->Render();

		m_pShaderCom->End();

		m_pGraphic_Device->SetTransform(D3DTS_VIEW, &SaveViewMatrix);
		m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &SaveProjVatrix);
	}

	if (FAILED(SetUp_RenderState()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OnGraphicDev()))
		return E_FAIL;

	TextureRender();

	m_pVIBufferCom->Render();

	if (FAILED(Release_RenderState()))
		return E_FAIL;

	On_SamplerState();

	if (g_bCollider)
		m_pColliderCom->Render();

	return S_OK;
}
HRESULT CMaiden::SetUp_Components(void)
{
	if (FAILED(__super::Add_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_VIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_IDLE_Front"), (CComponent**)&m_pTextureComIDLE_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_IDLE_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_IDLE_Back"), (CComponent**)&m_pTextureComIDLE_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_IDLE_Front"), (CComponent**)&m_pTextureComMove_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Move_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_IDLE_Back"), (CComponent**)&m_pTextureComMove_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_Attack_Front"), (CComponent**)&m_pTextureComAttack_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Attack_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_Attack_Back"), (CComponent**)&m_pTextureComAttack_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead_Front"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_Dead_Front"), (CComponent**)&m_pTextureComDead_Front)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Texture_Dead_Back"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_Dead_Back"), (CComponent**)&m_pTextureComDead_Back)))
		return E_FAIL;
	if (FAILED(__super::Add_Components(TEXT("Com_Collider"), LEVEL_STATIC, TEXT("Prototype_Component_Collider"), (CComponent**)&m_pColliderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_CharVIBuffer"), LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), (CComponent**)&m_pCharVIBufferCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_CharTexture"), LEVEL_STATIC, TEXT("Prototype_Component_Texture_Maiden_Talk"), (CComponent**)&m_pCharTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_Rect"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	CTransform::TRANSFORMDESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 1.5f;
	TransformDesc.fRotationPerSec = D3DXToRadian(90.f);

	if (FAILED(__super::Add_Components(TEXT("Com_Transform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Components(TEXT("Com_CharTransform"), LEVEL_STATIC, TEXT("Prototype_Component_Transform"), (CComponent**)&m_pCharTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaiden::SetUp_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 0);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	return S_OK;
}

HRESULT CMaiden::Release_RenderState(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	return S_OK;
}

void CMaiden::Check_Hit()
{
	if (m_tInfo.bHit)
	{
		CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTarget = this;
		tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);;
		tInfo.iTargetDmg = m_tInfo.iTargetDmg;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DmgFont"), LEVEL_MIDBOSS, TEXT("Layer_DmgFont"), &tInfo);
		tInfo.vPos = m_tInfo.vTargetPos;
		pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Hit"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo);
		CSoundMgr::Get_Instance()->PlayEffect(L"Hit_Sound.wav", fSOUND);
		m_tInfo.bHit = false;
		Safe_Release(pGameInstance);
	}
}

void CMaiden::Chase(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (Distance < 1.f)
		m_bStart = true;
	if (m_bStart)
	{
		if (m_fSkillCool > 5.f)
		{
			m_fSkillCool = 0.f;
			m_eCurState = SKILL;
			m_tFrame.iFrameStart = 0;
		}

		if (0.5f < Distance)
		{
			if (m_eCurState != SKILL)
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
			if (m_eCurState != SKILL)
				m_eCurState = IDLE;
			if (m_tFrame.iFrameStart > m_tFrame.iFrameEnd)
				m_tFrame.iFrameStart = m_tFrame.iFrameEnd;
			_float3 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
			//	vPosition.y = vTargetPos.y += 2.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
}


void CMaiden::OnTerrain()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CVIBuffer_Terrain*		pVIBuffer_Terrain = (CVIBuffer_Terrain*)pGameInstance->Get_Component(LEVEL_MIDBOSS, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"), 0);
	if (nullptr == pVIBuffer_Terrain)
		return;

	CTransform*		pTransform_Terrain = (CTransform*)pGameInstance->Get_Component(LEVEL_MIDBOSS, TEXT("Layer_BackGround"), TEXT("Com_Transform"), 0);
	if (nullptr == pTransform_Terrain)
		return;

	_float3			vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	if (m_eCurState == SKILL && m_tFrame.iFrameStart < 5)
	{
		m_fY += 0.05f;
		vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.7f + m_fY);
	}
	else if (m_eCurState == SKILL && m_tFrame.iFrameStart < 8 && m_tFrame.iFrameStart > 4)
	{

	}
	else
	{
		if (!m_bLastHeal)
			m_fY = 0.f;
		vPosition.y = pVIBuffer_Terrain->Compute_Height(vPosition, pTransform_Terrain->Get_WorldMatrix(), 0.7f + m_fY);
	}
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	Safe_Release(pGameInstance);
}

CMaiden * CMaiden::Create(LPDIRECT3DDEVICE9 _pGraphic_Device)
{
	CMaiden* pInstance = new CMaiden(_pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		ERR_MSG(TEXT("Failed to Created : CMaiden"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMaiden::Clone(void * pArg)
{
	CMaiden* pInstance = new CMaiden(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		ERR_MSG(TEXT("Failed to Cloned : CMaiden"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

_float4x4 CMaiden::Get_World(void)
{
	return m_pTransformCom->Get_WorldMatrix();
}

void CMaiden::Free(void)
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
	Safe_Release(m_pCharVIBufferCom);
	Safe_Release(m_pCharTransformCom);
	Safe_Release(m_pCharTextureCom);
	Safe_Release(m_pShaderCom);
}



HRESULT CMaiden::On_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return S_OK;
}

HRESULT CMaiden::Off_SamplerState()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);
	m_pGraphic_Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	return S_OK;
}

HRESULT CMaiden::Skill_DefaultAttack(const _tchar * pLayerTag)
{
	CGameInstance*			pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CGameObject::INFO tInfo;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	tInfo.iLevelIndex = LEVEL_MIDBOSS;
	tInfo.vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];


	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Sword"), LEVEL_MIDBOSS, pLayerTag, &tInfo)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}
void CMaiden::Motion_Change()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case IDLE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		case MOVE:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 5;
			m_tFrame.fFrameSpeed = 0.15f;
			break;
		case DEAD:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 4;
			m_tFrame.fFrameSpeed = 0.2f;
			m_bDead = true;
			break;
		case SKILL:
			m_tFrame.iFrameStart = 0;
			m_tFrame.iFrameEnd = 7;
			m_tFrame.fFrameSpeed = 0.1f;
			break;
		}

		m_ePreState = m_eCurState;
	}
}

void CMaiden::Move_Frame(_float fTimeDelta)
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
void CMaiden::Check_Front()
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
	}
	if ((((float)m_tInfo.iHp / (float)m_tInfo.iMaxHp) < 0.5f) && !m_bTotem)
	{

		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTerrain = this;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		tInfo.vPos = { 32.f,0.f,33.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Stone"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 6.f,0.f,33.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Stone"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 32.f,0.f,11.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Stone"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 6.f,0.f,11.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Stone"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		m_tInfo.iMp = 4;
		m_bTotem = true;
		Safe_Release(pGameInstance);
	}
	if ((((float)m_tInfo.iHp / (float)m_tInfo.iMaxHp) < 0.7f) && !m_bPowerTotem)
	{

		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTerrain = this;
		tInfo.pTarget = m_tInfo.pTarget;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		tInfo.vPos = { 19.f,0.f,21.f };

		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PowerTotem"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		m_tInfo.iMp = 1;
		m_bPowerTotem = true;
		Safe_Release(pGameInstance);
	}
	if ((((float)m_tInfo.iHp / (float)m_tInfo.iMaxHp) < 0.3f) && !m_bPowerTotem2)
	{

		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		tInfo.pTerrain = this;
		tInfo.pTarget = m_tInfo.pTarget;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;
		tInfo.vPos = { 19.f,0.f,21.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Stone"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 32.f,0.f,33.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PowerTotem"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 6.f,0.f,33.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PowerTotem"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 32.f,0.f,11.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PowerTotem"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		tInfo.vPos = { 6.f,0.f,11.f };
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_PowerTotem"), LEVEL_MIDBOSS, TEXT("Layer_Totem"), &tInfo)))
			return;
		m_tInfo.iMp = 5;
		m_bPowerTotem2 = true;
		m_bLastHeal = true;
		_float3 vPos = { 19.f,1.f,24.f };
		m_fY = 1.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
		Safe_Release(pGameInstance);
	}
	if (m_bLastHeal && m_tInfo.iMp == 0)
	{
		m_bBlueFire = true;
		m_bLastHeal = false;
		m_fSkillCool2 = 0;
		m_fSkillCool = 0;
	}

}
void CMaiden::Use_Skill(_float fTimeDelta)
{
	_float Distance = D3DXVec3Length(&(*(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (!m_bSkill && m_tFrame.iFrameStart == 5)
	{
		m_bSkill = true;
	}
	if (m_tFrame.iFrameStart == 7)
	{
		m_eCurState = IDLE;
		m_tFrame.iFrameStart = 0;
		m_fSkillCool = 0.f;
		Skill_DefaultAttack(TEXT("Layer_MonsterSkill"));
		m_bSkill = false;
	}
	if (m_tFrame.iFrameStart < 8 && m_tFrame.iFrameStart > 3 && Distance > 0.5f)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float3		vLook = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0] - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&vLook, &vLook) * 0.5f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	}
}
void CMaiden::Use_Skill2(_float fTimeDelta)
{
	if (m_iSkillMove == 0)
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition.z += 3.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vTargetPos.z -= 3.f;
		m_vTargetLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return;
	}
	if (m_iSkillMove < 22)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&m_vTargetLook, &m_vTargetLook) * 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		return;
	}
	if (m_iSkillMove == 22)
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition.x += 2.5f;
		vPosition.z -= 2.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vTargetPos.x -= 2.5f;
		vTargetPos.z += 2.5f;
		m_vTargetLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return;
	}
	if (m_iSkillMove > 22 && m_iSkillMove < 34)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&m_vTargetLook, &m_vTargetLook) * 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		return;
	}
	if (m_iSkillMove == 34)
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition.x -= 3.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vTargetPos.x += 3.f;
		m_vTargetLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return;
	}
	if (m_iSkillMove > 34 && m_iSkillMove < 46)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&m_vTargetLook, &m_vTargetLook) * 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		return;
	}
	if (m_iSkillMove == 46)
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition.x += 2.5f;
		vPosition.z += 2.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vTargetPos.x -= 2.5f;
		vTargetPos.z -= 2.5f;
		m_vTargetLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return;
	}
	if (m_iSkillMove > 46 && m_iSkillMove < 58)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&m_vTargetLook, &m_vTargetLook) * 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		return;
	}
	if (m_iSkillMove == 58)
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vPosition.x -= 2.5f;
		vPosition.z += 2.5f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		_float3 vTargetPos = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		vTargetPos.x += 2.5f;
		vTargetPos.z -= 2.5f;
		m_vTargetLook = vTargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		return;
	}
	if (m_iSkillMove > 58 && m_iSkillMove < 78)
	{
		_float3		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		vPosition += *D3DXVec3Normalize(&m_vTargetLook, &m_vTargetLook) * 0.3f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
		++m_iSkillMove;
		return;
	}
	if (m_iSkillMove == 78)
	{
		m_iSkillMove = 0;
		m_bSkill2 = false;
		return;
	}
}
HRESULT CMaiden::TextureRender()
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

void CMaiden::CheckColl()
{
	CGameInstance* pInstance = CGameInstance::Get_Instance();
	if (nullptr == pInstance)
		return;

	Safe_AddRef(pInstance);
	CGameObject* pTarget;

	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_PLAYER, TEXT("Com_Collider"), &pTarget))
	{
		_float3		vPosition = *(_float3*)&m_tInfo.pTarget->Get_World().m[3][0];
		pTarget->Set_Hp(444);
		pTarget->Set_Hit(444, vPosition);
	}
	if (pInstance->Collision(this, TEXT("Com_Collider"), COLLISION_TOTEM, TEXT("Com_Collider"), &pTarget))
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
	Safe_Release(pInstance);
}

void CMaiden::Create_BlueFire(_float fTimeDelta)
{
	m_bBlueFireTime += fTimeDelta;
	if (m_bBlueFireTime > 0.7f)
	{
		CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
		Safe_AddRef(pGameInstance);
		CGameObject::INFO tInfo;
		_float3 vPos = { 0.f,0.3f,0.f };



		_float fDest = rand() % 40000 * 0.001f;
		_float fSour = rand() % 40000 * 0.001f;

		tInfo.pTarget = m_tInfo.pTarget;
		tInfo.iLevelIndex = m_tInfo.iLevelIndex;

		vPos.x = fDest;
		vPos.z = fSour;
		tInfo.vPos = vPos;
		tInfo.fX = 1.f;
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_BossSkillTarget"), LEVEL_MIDBOSS, TEXT("Layer_Effect"), &tInfo)))
			return;

		Safe_Release(pGameInstance);
		m_bBlueFireTime = 0.f;
	}
}
void CMaiden::OnBillboard()
{
	_float4x4		ViewMatrix;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &ViewMatrix);

	D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
	_float3 vScale;
	_float3 vRight = *(_float3*)&ViewMatrix.m[0][0];
	_float3 vUp = *(_float3*)&ViewMatrix.m[1][0];

	if (m_bRight && m_bFront || m_bRight && !m_bFront)
	{
		m_pTransformCom->Set_Scaled(_float3(-2.5f, 2.5f, 1.f));
		vRight.x = -1;
	}
	else if (!m_bRight && !m_bFront || !m_bRight && m_bFront)
		m_pTransformCom->Set_Scaled(_float3(2.5f, 2.5f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, *D3DXVec3Normalize(&vRight, &vRight) * m_pTransformCom->Get_Scale().x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, *D3DXVec3Normalize(&vUp, &vUp) * m_pTransformCom->Get_Scale().y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, *(_float3*)&ViewMatrix.m[2][0]);
}
void CMaiden::DropItem()
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);
	CGameObject::INFO tInfo;
	tInfo.pTarget = this;
	tInfo.vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	tInfo.iLv = 3;

	pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HpPotion"), m_tInfo.iLevelIndex, TEXT("Layer_Item"), &tInfo);

	Safe_Release(pGameInstance);
}

void CMaiden::Ready_Script(void)
{
	m_vScript.push_back(TEXT("엔진을 찾으러 왔나? 어리석군..."));
	m_vScript.push_back(TEXT("어차피 곧 이 세계는 멸망하게 될 것이다.."));
	m_vScript.push_back(TEXT("네놈이 운명을 이길 수 있을 것 같으냐?"));
	m_vScript.push_back(TEXT("할 수 있으면 가져가 보아라!"));
}
